#include "cinn/lang/compute.h"

#include <gtest/gtest.h>

#include "cinn/ir/ir_operators.h"
#include "cinn/lang/placeholder.h"
#include "cinn/lang/tensor.h"

namespace cinn {
namespace lang {

TEST(Compute, basic) {
  Expr M(100);
  Expr N(100);

  Placeholder<float> x("x", {M, N});

  ir::Tensor y = Compute<compute_handle_2_t>({10, 20}, [&](Var i, Var j) -> Expr {
    LOG(INFO) << "type: " << x(i, j).type();
    return x(i, j) + 1.f;
  });
  LOG(INFO) << "compute: " << y->operaion->As<ir::ComputeOp>()->body[0];
}

}  // namespace lang
}  // namespace cinn
