namespace kProcessor{
void parseSequences(kmerDecoder * KD, kDataFrame* output);
void parseSequences(GenericDecoder * GD, kDataFrame* output);
void parseSequencesFromString(kmerDecoder *KD, string sequence,kDataFrame* output);
kDataFrame* kFrameUnion(const vector<kDataFrame*>& input);
kDataFrame* kFrameIntersect(const vector<kDataFrame*>& input);
kDataFrame* kFrameDiff(const vector<kDataFrame*>& input);
kmerDecoder* initialize_kmerDecoder(std::string filename, int chunkSize, std::string mode, std::map<std::string, int> params);
kmerDecoder* initialize_kmerDecoder(std::string mode, std::map<std::string, int> params);
GenericDecoder* initialize_genericDecoder(std::string _ofilePath, std::string source, MAPhasher* _hashMethod, int _minList, std::string _filterPath, std::string _dictionaryPath);
GenericDecoder* initialize_genericDecoder(std::string _ofilePath, std::string _ifilePath, std::string source, MAPhasher* _hashMethod, int _minList, std::string _filterPath, std::string _dictionaryPath);
GenericDecoder* initialize_genericDecoder(GenericDecoder* GD, std::string _ofilePath, std::string source, int _minList, std::string _filterPath, std::string _dictionaryPath);
GenericDecoder* initialize_genericDecoder(GenericDecoder* GD, std::string _ofilePath, std::string _ifilePath, std::string source, int _minList, std::string _filterPath, std::string _dictionaryPath);
GenericDecoder* initialize_genericDecoder(std::string _ifilePath, MAPhasher* _hashMethod);
GenericDecoder* set_hashMethod(GenericDecoder* GD, MAPhasher* _hashMethod);
GenericDecoder* set_minList(GenericDecoder* GD, int _minList);
GenericDecoder* set_filterPath(GenericDecoder* GD, std::string _filterPath);
GenericDecoder* set_dictionaryPath(GenericDecoder* GD, std::string _dictionaryPath);
GenericDecoder* decode(GenericDecoder* GD);
MAPhasher* initialize_MAPhasher(std::string filename);
void save_MAPhasher(MAPhasher* hashMethod, std::string filename);
colored_kDataFrame *index(kmerDecoder *KD, string names_fileName, kDataFrame *frame);
colored_kDataFrame *index(GenericDecoder *GD, string names_fileName, kDataFrame *frame);
}
