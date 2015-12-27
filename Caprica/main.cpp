
#include <ostream>

#include <papyrus/parser/PapyrusParser.h>
#include <papyrus/PapyrusScript.h>
#include <pex/PexWriter.h>

int main(int argc, char* argv[])
{
  auto parser = new caprica::papyrus::parser::PapyrusParser("test.psc");
  auto a = parser->parseScript();
  a->semantic();
  auto pex = a->buildPex();
  std::ofstream strm("test.pex", std::ofstream::binary);
  caprica::pex::PexWriter wtr(strm);
  pex->write(wtr);

  return 0;
}

