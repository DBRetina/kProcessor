# genericDecoder

genericDecoder is an external library that enables kProcessor to parse other files than FASTA files. The library would be updated to support different databases and different file formats.

## supported sources

- [DisGeNET](http://www.disgenet.org)
- [consensusPathDB](http://consensuspathdb.org)

## Parameters

1. filePath: string, path of the file that would be parsed. the file path must not include the
   default file format; such as ".tsv", ".json"
2. filterPath(optional): string, file path the contains a .json filter
3. source: string, one of the supported sources of file in parameter #1
4. hashMethod: MAPhasher pointer(optional), pointer to the hasher opject that store hashed values
5. fileNames: string, path of a tabular file that contains the generic names and their groups.
   "fileNames" is automatically created as "filePath"+".tsv.names" or "filePath"+"\_filtered.tsv.names"

## Initialization

### Initialize MAPhasher to load hashed values and save it

```python

hashMethod = kp.initialize_MAPhasher(filePath)

kp.save_MAPhasher(hashMethod, filePath)

```

### Initialize genericDecoder to parse a generic file

```python

GD = kp.initialize_genericDecoder(filePath, filterPath, source, hashMethod)

```

### Index a generic file

```python

# Initialize MAPhasher from old saved data
hashMethod = kp.initialize_MAPhasher(filePath)

# Initialize genericDecoder object
GD = kp.initialize_genericDecoder(filePath, source,hashMethod)

# Initialize kDataFrame object
KF = kp.kDataFrameMQF(21)

# Perform indexing, store elements with their colors in the KF object and the colors information will be returned as a colored_kDataFrame (cfk)
cfk = kp.index(GD, fileNames ,KF)

# save indexed data
cfk.save(filePath+"_indexed")

# save map of hashed values
kp.save_MAPhasher(hashMethod, filePath)

```
