# Gemma

Gemma is a file format to save any kind of data in a database-like way. It was created out of frustration by how Calibre manage files. Indeed, it saves e-books using the OS filesystem, which is slow when you have a few thousands of them.

Gemma is like one giant SQLite table. It can have metadata (owner, creation date, title, description, url, cover and compression) and user defined fields for each entries.

The layout of a Gemma file is the following (name: size of the attribute in bytes):
```
ᓚᘏᗢ + 8
size: 4
    owner: 4
        data: x
    title: 4
        data: x
    description: 4
        data: x
    url: 4
        data: x
    cover: 4
        data: x
    fields: 4
        data: x
    creation: 4
    compression type: 1
size: 4
    field 1: 4
        data: x
    field 2: 4
        data: x
    ...
    field n: 4
        data: x
```

Fields are named in the metadata and a field name starting with # mean that the data it host it compressed.

## Usage

Currently, compressing data is left to the user as, for my specific case, I have OPF (uncompressed) and EPUB (zip) file and compression on top of compression is useless.

``main.cpp`` contain a CLI tool which can be use to explore a Gemma file.

To get all the entrie which first attribute match with RegEx [a-zA-Z]+
```bash
Gemma -r "[a-zA-Z]+" -i 0 file.gemma
```

To get the entry at index 0:
```bash
Gemma -i 0 file.gemma
```

To display the metadata:
```bash
Gemma -m file.gemma
```

To display the metadata and the first entry:
```bash
Gemma -m -i 0 file.gemma
```

Code example:
```cpp
// Create the object.
Gemma g("test.gemma");

// Define the metadata.
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

// Write the file on the disk.
g.createFile();

// Open the file.
g.open();

// Print the metadata.
printf("%s", g.toString().c_str());

// Append entries to the end of the file.
g.appendEntry({"hello", "world"});
g.appendEntry({"hello", "world", "!!!"});
g.appendEntry({"hello"});
g.appendEntry({"Cogito", ", Ergo sum"});
g.appendEntry({"4568465", "984653211"});

// Map the file (optionnal).
g.buildMap();

// Print the size.
printf("Size: %i\n", g.size());

// Print all the entries.
for ( int i = 0; i < g.size(); ++i ) {
    std::vector<std::string> r = g.getEntryAtInd(i);
    printf("(%s, %s)\n", r[0].c_str(), r[1].c_str());
}
```

# Build

The implementation currently only use std. It should be compilable everywhere on any machine.

```bash
$ git clone https://github.com/j04chim/gemma.git
$ cd gemma
$ mkdir build
$ cmake .
$ make
```

The binary will be in the build folder. To remove the debug logs, change ``add_definitions(-DLOG_LEVEL=0)`` to ``add_definitions(-DLOG_LEVEL=3)`` or higher in ``CMakeLists.txt`` and re-run cmake.
