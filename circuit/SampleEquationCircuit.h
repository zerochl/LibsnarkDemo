
// r1cs_gg_ppzksnark 对应groth16方案，这里加了 gg 是为了区别 r1cs_ppzksnark（也就是 BCTV14a 方案），表示 Generic Group Model（通用群模型）
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
// 引入证明相关的各个接口
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
// 定义电路相关的变量
#include <libsnark/gadgetlib1/pb_variable.hpp>

using namespace libsnark;
using namespace std;

namespace sample {
    class EquationCircuit {
    public:
        // 定义使用的有限域
        typedef libff::Fr<default_r1cs_gg_ppzksnark_pp> FieldT;

        // ######################## 第一步 初始化电路
        // 待证命题 x^3 + x + 5 == out
        // 拆解为如下几个方程
        // x * x = sym_1
        // sym_1 * x = y
        // y + x = sym_2
        // sym_2 + 5 = out

        // 定义几个方程中涉及到的所有变量
        pb_variable<FieldT> x;
        pb_variable<FieldT> sym_1;
        pb_variable<FieldT> y;
        pb_variable<FieldT> sym_2;
        pb_variable<FieldT> out;


        EquationCircuit(protoboard<FieldT>& pb) {
            // 变量与电路板直接建立关系，相当于把组件插入到电路板上，allocate方法第二个参数类似于注释，方便查看日志
            out.allocate(pb, "out");
            x.allocate(pb, "x");
            sym_1.allocate(pb, "sym_1");
            y.allocate(pb, "y");
            sym_2.allocate(pb, "sym_2");
            // zkSNARKs需要区分public input与private witness，set_input_sizes用来干这个事情，输入参数为n（从1开始计数），表明前n个参数是public input，其余都是private
            pb.set_input_sizes(1);
        }

        void generate_r1cs_constraints(protoboard<FieldT>& pb) {
            // 组件以及插入到了电路板上，下面需要按照约束链接几个组件
            // 注意，此处r1cs_constraint方法只涉及乘法，如果要计算加法可以把sym_2 = y + x看成是sym_2 = (y + x) * 1，符合电路到QAP的变换知识
            // x*x = sym_1
            pb.add_r1cs_constraint(r1cs_constraint<FieldT>(x, x, sym_1));
            // sym_1 * x = y
            pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_1, x, y));
            // y + x = sym_2
            pb.add_r1cs_constraint(r1cs_constraint<FieldT>(y + x, 1, sym_2));
            // sym_2 + 5 = ~out
            pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_2 + 5, 1, out));
        }

        void generate_r1cs_witness(protoboard<FieldT>& pb) {
            // 为public input和private witness设置值，x=3，out=35，其他的按照逻辑关系求解即可
            pb.val(out) = 35;

            pb.val(x) = 3;
            pb.val(sym_1) = 9;
            pb.val(y) = 27;
            pb.val(sym_2) = 30;
        }

        r1cs_gg_ppzksnark_keypair<default_r1cs_gg_ppzksnark_pp> setup(protoboard<FieldT>& pb) {
            const r1cs_constraint_system<FieldT> constraint_system = pb.get_constraint_system();
            return r1cs_gg_ppzksnark_generator<default_r1cs_gg_ppzksnark_pp>(constraint_system);
        }
    };
}