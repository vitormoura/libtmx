# libtmx
Biblioteca C++ para leitura de arquivos tmx gerados pelo Tiled Map Editor (Versão preliminar).  

Internamente, a biblioteca utiliza o [rapidxml](http://rapidxml.sourceforge.net/) para leitura dos arquivos XML. 

### Utilização

```cpp
#include "LibTMX/map.h"

// ...

using namespace tmxparser;

tmxparser::map<> map;
map.load("./Assets/lab_map_002.tmx");

```

### Tiled Map Editor

Documentação do Tiled Map Editor  
[http://doc.mapeditor.org/en/latest/](http://doc.mapeditor.org/en/latest/)

Formato arquivo TMX  
[http://doc.mapeditor.org/en/latest/reference/tmx-map-format/](http://doc.mapeditor.org/en/latest/reference/tmx-map-format/)

### Testes  

Para viabilizar a execução dos testes unitários você deve configurar as dependências da biblioteca gtest. Você pode utilizar os binários de gtest presentes na pasta `libs` ou clonar e compilar a biblioteca você mesmo. Em seguida, edite o arquivo `src/gtest.props` para definir a localização dessas libs em seu computador.

Caso não tenha interesse em utilizar os testes, remova ou desative o build desse projeto.

Saiba mais sobre a biblioteca gtest em [https://github.com/google/googletest](https://github.com/google/googletest)
