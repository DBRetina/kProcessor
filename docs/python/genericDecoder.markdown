# genericDecoder

genericDecoder is an external library that enables kProcessor to parse other files than FASTA files. The library would be updated to support different databases and different file formats.

## supported sources

- [DisGeNET](http://www.disgenet.org)

## Parameters

1. string, path of the file that would be parsed. the file path must not include the default file format; such as ".tsv", ".json"
2. string, the source of file in parameter #1.

## Initialization

### Initialize to parse a generic file

```python

# File to be parsed
filename = ".../.../sample"

# The source of "filename"
source = "disgenet"

GD = kp.initialize_genericDecoder(filename, source)

```

### Index a generic file

```python

# Initialize genericDecoder object

# File to be parsed
filename = ".../.../sample"

# The source of "filename"
source = "disgenet"

GD = kp.initialize_genericDecoder(filename, source)

# Initialize kDataFrame object
KF = kp.kDataFrameMQF()

#  "sourceNames": contains two columns, one column for names of the lists in
#  "filename", the second column is to define the group of the first column.
sourceNames = ".../.../sample.names"

# Perform indexing, store elements with their colors in the KF object and the colors information will be returned as a colored_kDataFrame (cfk)
cfk = kp.index(GD, sourceNames ,KF)

cfk.save(filename+"_indexed")

```
