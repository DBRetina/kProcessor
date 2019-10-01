# genericDecoder

genericDecoder is an external library that enables kProcessor to parse other files than FASTA files. The library would be updated to support different databases and different file formats.

## supported sources

- [DisGeNET](http://www.disgenet.org)
- [consensusPathDB](http://consensuspathdb.org)

## Parameters

1. GD(optional): genericDecoder pointer, genericDecoder that would be appended to the current opject
2. ofilePath: string, path of the directory that would contains the output files
3. ifilePath(optional): string, path of the file that would be parsed. the file path must not include the default file format; such as ".tsv", ".json"
4. source: string, one of the supported sources of file in parameter #2
5. hashMethod(optional): MAPhasher pointer(optional), pointer to the hasher opject that store hashed values
6. minList(optional): int, the minimum number of children
7. filterPath(optional): string, file path the contains a .json filter
8. dictionaryPath(optional): string, file path that contains two columns of a word and its meaning to translate children
9. fileNames: string, path of a tabular file that contains the generic names and their groups. "fileNames" is automatically created as "filePath"+".tsv.names" or "filePath"+"\_filtered.tsv.names"

## Initialization

### Initialize MAPhasher to load hashed values and save it

```python

hashMethod = kp.initialize_MAPhasher(ifilePath)

kp.save_MAPhasher(hashMethod, ifilePath)

```

### Initialize genericDecoder to download the last version from the database

```python

GD = kp.initialize_genericDecoder(ofilePath, source)
GD = kp.decode(GD)

```

### Initialize genericDecoder to store a json file of asscotiation list

```python

GD = kp.initialize_genericDecoder(aListFilePath)
GD = kp.decode(GD)

```

### Index a generic file

```python

# Initialize MAPhasher from old saved data
hashMethod = kp.initialize_MAPhasher(filePath)

# Initialize genericDecoder object
GD = kp.initialize_genericDecoder(ofilePath, ifilePath, source ,hashMethod)

# parse file
GD = kp.decode(GD)

# Initialize kDataFrame object
KF = kp.kDataFrameMQF(21)

# Perform indexing, store elements with their colors in the KF object and the colors information will be returned as a colored_kDataFrame (cfk)
cfk = kp.index(GD, fileNames ,KF)

# Save indexed data
cfk.save(filePath+"_indexed")

# Save map of hashed values
kp.save_MAPhasher(hashMethod, filePath)

```

### Add some options to the genericDecoder

```python

# Initialize MAPhasher from old saved data
hashMethod = kp.initialize_MAPhasher(filePath)

# Initialize genericDecoder object
GD = kp.initialize_genericDecoder(ofilePath, ifilePath, source)

# Add hashMethod (optional)
GD = kp.set_hashMethod(GD, hashMethod)

# Add minList (optional)
GD = set_minList(GD, minList)

# Add filterPath (optional)
GD = set_filterPath(GD, filterPath)

# Add dictionaryPath (optional)
GD = set_dictionaryPath(GD, dictionaryPath)

# parse file
GD = kp.decode(GD)


```
