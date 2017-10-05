# libtmx
Biblioteca C++ para leitura de arquivos tmx gerados pelo Tiled Map Editor. Essa biblioteca ainda está em desenvolvimento, funcionando como laboratório para meus experimentos em c++, ainda não tem a proposta de ser um modelo de boas práticas ou desempenho. Consulte a [documentação do Tiled](http://doc.mapeditor.org/en/latest/reference/support-for-tmx-maps/) para conhecer bibliotecas mais maduras para carga de arquivos TMX.

Internamente, a libtmx utiliza o [rapidxml](http://rapidxml.sourceforge.net/) para leitura dos arquivos XML. 

### Utilização

Por se tratar de uma biblioteca formada apenas por arquivo .h, basta incluir seus arquivos ao código do seu projeto e referenciar o namespace `tmxparser`:

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


### Amostras

O diretório `samples` demonstra a utilização da biblioteca para carregar um mapa qualquer utilizando SDL. Para executá-lo, modifique os arquivos `.prop` para identificar a localização das bibliotecas do SDL (e do próprio libtmx) em seu computador, e copie as DLLs do SDL para o diretório de execução (geralmente Debug/x64 ou Debug/x86).

Você pode obter as bibliotecas do SDL2 no [site oficial](https://www.libsdl.org/download-2.0.php), SDL_Image em [https://www.libsdl.org/projects/SDL_image/](https://www.libsdl.org/projects/SDL_image/) e a biblioteca `SDL2_gfx` em [https://sourceforge.net/projects/sdl2gfx/](https://sourceforge.net/projects/sdl2gfx/).