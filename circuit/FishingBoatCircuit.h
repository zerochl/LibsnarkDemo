// r1cs_gg_ppzksnark 对应groth16方案，这里加了 gg 是为了区别 r1cs_ppzksnark（也就是 BCTV14a 方案），表示 Generic Group Model（通用群模型）
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
// 引入证明相关的各个接口
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
// 定义电路相关的变量
#include <libsnark/gadgetlib1/pb_variable.hpp>
// 基础库，当前用到comparison_gadget
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>

using namespace libsnark;
using namespace std;

namespace sample {
    class FishingBoatCircuit {
    public:
        // 定义使用的有限域
        typedef libff::Fr<default_r1cs_gg_ppzksnark_pp> FieldT;
        // 比较两个数大小没有直接使用的门逻辑，所以需要进行转化，libsnark 把 A 和 B 的比较，转化为 2^n + B - A 按位表示。
        // 具体可以参考已经提供的工具：basic_gadgets中的comparison_gadget
        // comparison_gadget(protoboard<FieldT>& pb,
//                               const size_t n,
//                               const pb_linear_combination<FieldT> &A,
//                               const pb_linear_combination<FieldT> &B,
//                               const pb_variable<FieldT> &less,
//                               const pb_variable<FieldT> &less_or_eq,
//                               const std::string &annotation_prefix="")
        // 公共电路输入，表示合法捕捞区域的纬度的最小值和最大值，数组
        pb_variable<FieldT> latitudeRange0, latitudeRange1;
        // 公共电路输入，表示合法捕捞区域的经度的最小值和最大值，数组
        pb_variable<FieldT> longitudeRange0, longitudeRange1;
        // 公共电路输出，1表示渔船在合法捕捞区，0表示渔船不在合法捕捞区
        pb_variable<FieldT> out;
        // 隐私电路输入，表示渔船的纬度和经度，数组,(latitude, longitude)
        pb_variable<FieldT> fishingLocation0, fishingLocation1;

        // 1: fishingLocation[0] > latitudeRange[0]
        // 2: fishingLocation[0] < latitudeRange[1]
        // 3: fishingLocation[1] > longitudeRange[0]
        // 4: fishingLocation[1] < longitudeRange[1]
        pb_variable<FieldT> lessOne, lessOrEqOne;
        pb_variable<FieldT> lessTwo, lessOrEqTwo;
        pb_variable<FieldT> lessThree, lessOrEqThree;
        pb_variable<FieldT> lessFour, lessOrEqFour;

//        comparison_gadget<FieldT> *cmpOne;
        std::shared_ptr<comparison_gadget<FieldT>> cmpOne;
        std::shared_ptr<comparison_gadget<FieldT>> cmpTwo;
        std::shared_ptr<comparison_gadget<FieldT>> cmpThree;
        std::shared_ptr<comparison_gadget<FieldT>> cmpFour;

        FishingBoatCircuit(protoboard<FieldT>& pb) {
            // 变量与电路板直接建立关系，相当于把组件插入到电路板上，allocate方法第二个参数类似于注释，方便查看日志
            latitudeRange0.allocate(pb, "latitudeSmall");
            latitudeRange1.allocate(pb, "latitudeBig");

            longitudeRange0.allocate(pb, "longitudeSmall");
            longitudeRange1.allocate(pb, "longitudeBig");

            out.allocate(pb, "out");

            fishingLocation0.allocate(pb, "fishingLatitude");
            fishingLocation1.allocate(pb, "fishingLongitude");

            // zkSNARKs需要区分public input与private witness，set_input_sizes用来干这个事情，输入参数为n（从1开始计数），表明前n个参数是public input，其余都是private
            pb.set_input_sizes(5);
        }

        void generate_r1cs_constraints(protoboard<FieldT>& pb) {
            // 组件以及插入到了电路板上，下面需要按照约束链接几个组件
            // 梳理几个关系
            // 1: fishingLocation[0] > latitudeRange[0]
            // 2: fishingLocation[0] < latitudeRange[1]
            // 3: fishingLocation[1] > longitudeRange[0]
            // 4: fishingLocation[1] < longitudeRange[1]
//            // 5: out = 1 * 1
//            pb.add_r1cs_constraint(r1cs_constraint<FieldT>(1, 1, out));


        // comparison_gadget(protoboard<FieldT>& pb,
//                               const size_t n,
//                               const pb_linear_combination<FieldT> &A,
//                               const pb_linear_combination<FieldT> &B,
//                               const pb_variable<FieldT> &less,
//                               const pb_variable<FieldT> &less_or_eq,
//                               const std::string &annotation_prefix="")

            cmpOne.reset(new comparison_gadget<FieldT>(pb, 10, latitudeRange0, fishingLocation0, lessOne, lessOrEqOne, "cmpOne"));
            cmpOne.get()->generate_r1cs_constraints();
            // lessOne * 1 = 1，也就是 lessOne 必须为 true
            pb.add_r1cs_constraint(r1cs_constraint<FieldT>(lessOne, 1, FieldT::one()));

            cmpTwo.reset(new comparison_gadget<FieldT>(pb, 10, fishingLocation0, latitudeRange1, lessTwo, lessOrEqTwo, "cmpTwo"));
            cmpTwo.get()->generate_r1cs_constraints();
            // lessTwo * 1 = 1，也就是 lessTwo 必须为 true
            pb.add_r1cs_constraint(r1cs_constraint<FieldT>(lessTwo, 1, FieldT::one()));

//            cmpThree = new comparison_gadget<FieldT>(pb, 10, fishingLocation0, latitudeRange1, lessThree, lessOrEqThree, "cmpThree");
            cmpThree.reset(new comparison_gadget<FieldT>(pb, 10, fishingLocation0, latitudeRange1, lessThree, lessOrEqThree, "cmpThree"));
            cmpThree.get()->generate_r1cs_constraints();
            // lessThree * 1 = 1，也就是 lessThree 必须为 true
            pb.add_r1cs_constraint(r1cs_constraint<FieldT>(lessThree, 1, FieldT::one()));

//            cmpFour = new comparison_gadget<FieldT>(pb, 10, fishingLocation0, latitudeRange1, lessFour, lessOrEqFour, "cmpFour");
            cmpFour.reset(new comparison_gadget<FieldT>(pb, 10, fishingLocation0, latitudeRange1, lessFour, lessOrEqFour, "cmpFour"));
            cmpFour.get()->generate_r1cs_constraints();
            // lessFour * 1 = 1，也就是 lessFour 必须为 true
            pb.add_r1cs_constraint(r1cs_constraint<FieldT>(lessFour, 1, FieldT::one()));

            // 此关系可能没有必要了
            pb.add_r1cs_constraint(r1cs_constraint<FieldT>((lessOne + lessTwo + lessThree + lessFour), 1, out));

        }

        void generate_r1cs_witness(protoboard<FieldT>& pb) {
            // 公共电路输入，表示合法捕捞区域的纬度的最小值, 最大值
            pb.val(latitudeRange0) = 30;
            pb.val(latitudeRange1) = 60;
            // 公共电路输入，表示合法捕捞区域的经度的最小值和最大值
            pb.val(longitudeRange0) = 40;
            pb.val(longitudeRange1) = 60;

            pb.val(out) = 4;

            // 隐私电路输入，表示渔船的纬度和经度，数组,(latitude, longitude)
            pb.val(fishingLocation0) = 40;
            pb.val(fishingLocation1) = 50;

            cmpOne.get()->generate_r1cs_witness();
            cmpTwo.get()->generate_r1cs_witness();
            cmpThree.get()->generate_r1cs_witness();
            cmpFour.get()->generate_r1cs_witness();
        }

        r1cs_gg_ppzksnark_keypair<default_r1cs_gg_ppzksnark_pp> setup(protoboard<FieldT>& pb) {
            const r1cs_constraint_system<FieldT> constraint_system = pb.get_constraint_system();
            return r1cs_gg_ppzksnark_generator<default_r1cs_gg_ppzksnark_pp>(constraint_system);
        }
    };
}