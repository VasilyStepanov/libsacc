#include "test_mpool.h"
#include "test_list.h"
#include "test_lexical_unit.h"
#include "test_parser.h"

#include <stdlib.h>



int main() {
  test_mpool();
  test_list();
  test_lexical_unit();
  test_parser();
  return EXIT_SUCCESS;
}
