/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "connection.h"

#include <iostream>

#include <QHostInfo>
#include <QTextStream>
#include <QTcpSocket>
#include <QTextStream>
#include <QFile>

#include <KProcess>

#include "common.h"
#include "settings.h"

Connection::Connection()
: m_device(NULL)
, m_connected(false)
, m_initialized(false) {
  QString logFileName;
  if (!logFileName.isEmpty()) {
    logFile = new QFile(logFileName);
    logFile->open(QIODevice::WriteOnly);
    logStream = 0; //= new QTextStream(logFile);
  }
  else
    logStream = 0;
}

Connection::~Connection() {
  if (logStream) {
    delete logStream;
    logFile->close();
    delete logFile;
  }
  if (m_device) {
    KProcess* p = qobject_cast<KProcess*>(m_device);
    if (p)
      p->kill();
    delete m_device;
  }
}

void Connection::establish(const char* host, quint16 port,
                  const QString& helper, const QString& helper_cmd) {
  if (m_device) {
    KProcess* p = qobject_cast<KProcess*>(m_device);
    if (p)
      p->kill();
    delete m_device;
  }

  strncpy(m_host, host, 256);
  m_port = port;
  m_helper = helper;
  m_helper_cmd = helper_cmd;

  if (helper.isEmpty()) {
    QTcpSocket *s = new QTcpSocket(this);
    s->setObjectName("IcsSocket");

    connect(s, SIGNAL(hostFound()), this, SIGNAL(hostFound()));
    connect(s, SIGNAL(connected()), this, SIGNAL(established()));

    hostLookup();
    s->connectToHost(m_host, m_port);

    m_device = s;
    m_connected = true;
  }
  else {
    m_device = new KProcess(this);
    m_device->setObjectName("IcsHelper");

    if (m_helper_cmd.contains("$(HOST_IP)")) {
      QHostInfo::lookupHost(m_host, this, SLOT(lookedUp(const QHostInfo)));
    }
    else {
      lookedUp(QHostInfo());
    }
  }

  connect(m_device, SIGNAL(readyRead()), this, SLOT(processLine()));
}

void Connection::lookedUp(const QHostInfo &hi) {
  KProcess* p = m_device ? qobject_cast<KProcess*>(m_device) : NULL;
  if (p) {
    QString args = m_helper_cmd;
    args.replace("$(HOST_NAME)", QString(m_host) );
    args.replace("$(HOST_IP)", hi.addresses().isEmpty() ? QString() :
                                    hi.addresses().first().toString () );
    args.replace("$(PORT)", QString::number(m_port));
    p->setOutputChannelMode(KProcess::OnlyStdoutChannel);
    p->setProgram(m_helper, args.split(' '));
    p->start();
  
    m_connected = true;
    if(!m_unsent_text.isEmpty()) {
      QTextStream os(m_device);
      os << m_unsent_text;
  
      if (logStream)
        (*logStream) << "> " << m_unsent_text;
      m_unsent_text = QString();
    }
  }
}

void Connection::processLine() {
  if(!m_device) {
    ERROR("No m_device");
    return;
  }

  while (m_device->canReadLine()) {
    QString line = m_device->readLine();
    line = buffer + line.replace("\r", "");
    receivedLine(line, buffer.length());
    if (logStream)
      (*logStream) << "< " << line << "\n";
    buffer = "";
  }

  int size;
  if ((size = m_device->bytesAvailable()) > 0) {
    char* data = new char[size + 1];
    m_device->read(data, size);
    data[size] = '\0';
    int offset = buffer.length();
    buffer += QString(data).replace("\r", "");
    delete[] data;

    receivedText(buffer, offset);
  }
}

void Connection::sendText(const QString& text) {
  if(!m_connected) {
    m_unsent_text += text + "\n";
    return;
  }

  if(!m_device) {
    ERROR("No m_device");
    return;
  }

  processLine();
  QTextStream os(m_device);
  os << text << "\n";

  if (logStream)
    (*logStream) << "> " << text << "\n";
}



