#include <stdio>
#include <stdheap>
#include <stdstring>

int32 main = (int16 argc, []byte* argv) {
  -- Faz a coleta os argumentos 1 e 2
  []byte first = argv[1];
  []byte second = argv[2];
  []byte msg = "Hello, world!"; 

  -- Aloca 128 bytes como destino da junção dos valores dos argumentos 1 e 2
  []byte buffer = heap::alloc(128);

  -- Junta a string do argumento 1 com a do 2 (Caso a 1 tenha placeholders).
  string::format(buffer, first, second);

  -- Printa os argumentos já juntos de acordo com as placeholders
  io::println(buffer);
  
  -- Limpa a memória reservada para o buffer
  heap::dump(buffer);

  -- Printa o conteúdo após a limpeza (Conteudo perdido)
  io::println(buffer);
  
  return 0;
}