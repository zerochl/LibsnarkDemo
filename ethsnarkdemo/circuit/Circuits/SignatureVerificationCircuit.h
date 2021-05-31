// SPDX-License-Identifier: Apache-2.0
// Copyright 2017 Loopring Technology Limited.
#ifndef _SIGNATUREVERIFICATIONCIRCUIT_H_
#define _SIGNATUREVERIFICATIONCIRCUIT_H_

#include "Circuit.h"
#include "../Utils/Constants.h"
#include "../Utils/Data.h"

#include "ethsnarks.hpp"
#include "utils.hpp"

#include "../Gadgets/SignatureGadgets.h"

using namespace ethsnarks;

namespace Loopring
{

class SignatureVerificationCircuit : public BaseTransactionCircuit
{
  public:
    // Inputs
    // ToBitsGadget 与 DualVariableGadget均继承自libsnark::dual_variable_gadget，初始化参数不一样，方法实现不一样
    ToBitsGadget owner;
    DualVariableGadget accountID;
    DualVariableGadget data;

    SignatureVerificationCircuit( //
      ProtoboardT &pb,
      const TransactionState &state,
      const std::string &prefix)
        : BaseTransactionCircuit(pb, state, prefix),

          // Inputs
          // NUM_BITS_ADDRESS = 160
          // owner的初始化提供了owner参数，accountID和data并没有
          owner(pb, state.accountA.account.owner, NUM_BITS_ADDRESS, FMT(prefix, ".owner")),
          // NUM_BITS_ACCOUNT = TREE_DEPTH_ACCOUNTS * 2; TREE_DEPTH_ACCOUNTS = 12;
          accountID(pb, NUM_BITS_ACCOUNT, FMT(prefix, ".accountID")),
          // NUM_BITS_FIELD_CAPACITY = 253
          data(pb, NUM_BITS_FIELD_CAPACITY, FMT(prefix, ".data"))
    {
        // TXV_ACCOUNT_A_ADDRESS 枚举数据，可以看做是key
        // 设置TXV_ACCOUNT_A_ADDRESS与accountID bits的映射关系，bits是pb_variable_array<FieldT>类型
        setArrayOutput(TXV_ACCOUNT_A_ADDRESS, accountID.bits);

        // We need a single signature of the account
        // TXV_HASH_A、TXV_SIGNATURE_REQUIRED_A、TXV_SIGNATURE_REQUIRED_B均是枚举，可以看做是key
        // 建立data packed的映射，packed是pb_variable<FieldT>类型
        setOutput(TXV_HASH_A, data.packed);
        // 建立state.constants._1的映射，state.constants._1是VariableT，也就是pb_variable<FieldT>
        setOutput(TXV_SIGNATURE_REQUIRED_A, state.constants._1);
        // 建立state.constants._0的映射，state.constants._0是VariableT，也就是pb_variable<FieldT>
        setOutput(TXV_SIGNATURE_REQUIRED_B, state.constants._0);
    }

    void generate_r1cs_witness(const SignatureVerification &verification)
    {
        // Inputs
        // owner好像并没有填充任何值，初始化的时候传入的已经是传过值的数据了
        owner.generate_r1cs_witness();
        // accountID的packed传值，accountID：ethsnarks::FieldT
        accountID.generate_r1cs_witness(pb, verification.accountID);
        // data的packed传值，data：ethsnarks::FieldT
        data.generate_r1cs_witness(pb, verification.data);
    }

    void generate_r1cs_constraints()
    {
        // Inputs
        owner.generate_r1cs_constraints();
        accountID.generate_r1cs_constraints(true);
        data.generate_r1cs_constraints(true);
    }

    const VariableArrayT getPublicData() const
    {
        return flattenReverse({owner.bits, accountID.bits, VariableArrayT(3, state.constants._0), data.bits});
    }
};

} // namespace Loopring

#endif
