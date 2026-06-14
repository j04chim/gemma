#include "gemma.h"
#include <cstdio>

int main()
{
    Gemma g("test.gemma");

    g.setCompression(1);
    g.setCover("cover");
    g.setCreation(123456789);
    g.setDescription("A library");
    g.setOwner("Test");
    g.setUrl("https://abc/");
    g.setTitle("Abc");
    g.addField("test");
    g.addField("test2");
    g.toString();

    g.createFile();
    g.open();
    printf("%s", g.toString().c_str());

    g.appendEntry({"hello", "world"});
    g.appendEntry({"hello", "world", "!!!"});
    g.appendEntry({"hello"});
    g.appendEntry({"Cogito", ", Ergo sum"});
    g.appendEntry({"4568465", "984653211"});

    g.buildMap();
    printf("Size: %i\n", g.size());
    for ( int i = 0; i < g.size(); ++i ) {
        std::vector<std::string> r = g.getEntryAtInd(i);
        printf("(%s, %s)\n", r[0].c_str(), r[1].c_str());
    }

}