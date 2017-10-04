#include <gtest\gtest.h>
#include "../LibTMX/map.h"

using namespace tmxparser;

TEST(Tests_FilePath, CargaArquivoMesmaPastaIdentificaFilePathCorretamente) {
	tmxparser::map<> map;
	map.load("lab_map_001.tmx");

	ASSERT_STREQ(map.file_path.first.c_str(), "");
	ASSERT_STREQ(map.file_path.second.c_str(), "lab_map_001.tmx");
}

TEST(Tests_FilePath, CargaArquivoPastaSemArquivoIdentificaFilePathCorretamente) {
	tmxparser::map<> map;
	map.load("./pasta/");

	ASSERT_STREQ(map.file_path.first.c_str(), "./pasta");
	ASSERT_STREQ(map.file_path.second.c_str(), "");
}

TEST(Tests_FilePath, CargaArquivoDiretorioLocalIdentificaFilePathCorretamente) {
	tmxparser::map<> map;
	map.load("./lab_map_001.tmx");

	ASSERT_STREQ(map.file_path.first.c_str(), "");
	ASSERT_STREQ(map.file_path.second.c_str(), "lab_map_001.tmx");
}

TEST(Tests_FilePath, CargaArquivoPastaComportaIdentificaFilePathCorretamente) {
	tmxparser::map<> map;
	map.load("local1/local2/local3/lab_map_001.tmx");

	ASSERT_STREQ(map.file_path.first.c_str(), "local1/local2/local3");
	ASSERT_STREQ(map.file_path.second.c_str(), "lab_map_001.tmx");
}

TEST(Tests_FilePath, CargaArquivoNaoInformadoNaoPreencheFilePath) {
	tmxparser::map<> map;
	map.load("");

	ASSERT_TRUE(map.file_path.first.size() == 0);
	ASSERT_TRUE(map.file_path.second.size() == 0);
}
