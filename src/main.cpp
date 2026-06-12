#include "gemma.h"

int main()
{
    Gemma g;

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

    g.open("test.gemma");
    g.createFile();
    g.open("test.gemma");
    printf("%s", g.toString().c_str());

}