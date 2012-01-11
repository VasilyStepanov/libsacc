#include "test_mpool.h"
#include "test_list.h"
#include "test_vector.h"
#include "test_lexical_unit.h"
#include "test_condition.h"
#include "test_selector.h"
#include "test_parser.h"

#include <stdlib.h>



int main() {
  test_mpool();
  test_list();
  test_vector();
  test_lexical_unit();
  test_condition();
  test_selector();
  test_parser();
  return EXIT_SUCCESS;
}
