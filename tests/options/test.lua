
-- options = OptList {
--   BoolOpt("pippo", "Pluto", true),
--   BoolOpt("gnugnu", "Jerry", false, OptList{
--     BoolOpt("zio", "Porco!", true)
--   })
-- }

options = OptList {
    BoolOpt("teto", "Teto?", true),
    BoolOpt("fufi", "Fufi?", true, OptList{
        BoolOpt("gni", "Gni gni?", true, OptList {
            ColorOpt("pappa", "Pappa?", "red"),
            FontOpt("ciccia", "Ciccia!", "Monospace")
        }),
        BoolOpt("gnu", "Gnu gnu?", false)
    }),
    IntOpt("ratta", "Ratta:", 5, 0, 10),
    ComboOpt("schiatta", "Fai a Paolo:", {"Incula", "Impala", "Spezza","Uccidi", "Tortura"}),
    SelectOpt("affa", "Affanculo:", BoolOptList {
        BoolOpt("ght", "Case1", true, OptList {
            BoolOpt("gni2", "Gni gni?", true)
        }),
        BoolOpt("cvb", "Case2", true, OptList {
            StringOpt("sheshe", "She She:", "Sho shu?"),
            UrlOpt("shesdsshe", "She She:", "Sho shu?")
        })
    })
}

-- print(options)
-- print(options.pippo.value)
-- print(options.pippo.name)
-- print(options.pippo.label)
-- print(options.gnugnu.sub_options)
-- print(options.gnugnu.sub_options.zio.value)
