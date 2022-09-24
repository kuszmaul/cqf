#include "counting_quotient_filter.h"

#include "experimental/users/bradleybear/cqf/include/test.h"
#include "testing/base/public/gmock.h"
#include "testing/base/public/gunit.h"

namespace counting_quotient_filter {

TEST(CountingQuotientFilter, Ctest) {
  cqf_test(21, 8);
}

}  // namespace counting_quotient_filter
