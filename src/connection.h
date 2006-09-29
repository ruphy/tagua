/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

class QTextStream;
class QIODevice;
class QFile;
class QHostInfo;
class ChessMove;

/**
  * @brief Manage a connection with a remote server.
  *
  * This class can be used to manage a connection with a chess server. Once connected,
  * it emits the signal @ref receivedLine whenever a new whole line is received.
  * Partial lines are kept in an internal buffer.
  * The Connection class supports the use of a helper such as timeseal.
  */
class Connection : public QObject {
Q_OBJECT
  QIODevice* m_device;
  QString buffer;
  QFile* logFile;
  QTextStream* logStream;
  bool m_connected;
  bool m_initialized;
  QString m_unsent_text;

  char m_host[256];
  quint16 m_port;
  QString m_helper;
  QString m_helper_cmd;

public:
  /**
    * Create a new connection object. No actual connection is made
    * until @ref establish is called.
    */
  Connection();

  /**
    * Destroy the object and kill the helper, if present.
    */
  virtual ~Connection();

  /**
    * Create an actual connection to an internet host.
    * @param host Fully qualified name of the host.
    * @param port Host port to connect to.
    * @param helper Full path of a helper. A null or empty string if no helper is needed.
    * @param args Helper arguments. They can optionally contain variabiles which will be expaded
    *             before they are passed to the helper.
    *             Supported variables: $(HOST_NAME), $(HOST_IP), $(PORT).
    */
  void establish(const char* host, quint16 port,
                  const QString& helper, const QString& args);

  /**
    * Whether the connection has been initialized.
    */
  inline bool initialized() const { return m_initialized; }

  /**
    * Set the inizialized flag of the connection. Use this after you have
    * sent initial commands.
    */
  inline void setInitialized() { m_initialized = true; }

public slots:
  /**
    * Send some text to the server using the connection. A newline will be sent afterwards.
    */
  void sendText(const QString&);
  void lookedUp(const QHostInfo &host);

protected slots:
  /**
    * Read data from the socket and emit appropriate signals.
    */
  void processLine();

signals:
  void hostLookup();
  void hostFound();
  void established();

  /**
    * Emitted when a partial line is received.
    *
    * @param text Received text.
    * @param newDataOffset Offset where new data starts.
    * @note  Whenever a partial line is received, is it recorded in an internal buffer,
    *        and an offset is incremented, to distinguish new data from old data
    *        as the next payload arrives.
    */
  void receivedText(QString text, int newDataOffset);

  /**
    * Emitted when a full line is received.
    *
    * @param line Received line.
    * @param newDataOffset Offset where new data starts.
    *
    * @sa receivedText
    */
  void receivedLine(QString line, int newDataOffset);
};

#endif // CONNECTION_H
