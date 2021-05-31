#ifndef _DEPOSITCIRCUIT_H_
#define _DEPOSITCIRCUIT_H_

#include "Circuit.h"
#include "../../loopring/utils/Constants.h"
#include "../../loopring/utils/Data.h"

#include "../../loopring/thirdpart/BigIntHeader.hpp"

#include "src/ethsnarks.hpp"
#include "src/utils.hpp"
#include "src/gadgets/sha256_many.hpp"


#include "../../loopring/gadgets/MathGadgets.h"
#include "../../loopring/gadgets/AccountGadgets.h"

using namespace ethsnarks;

namespace Loopring
{

class DepositGadget : public GadgetT
{
public:

    const Constants& constants;

    // User state
    BalanceGadget balanceBefore;
    AccountGadget accountBefore;

    // Inputs
    DualVariableGadget accountID;
    DualVariableGadget tokenID;
    DualVariableGadget amount;
    DualVariableGadget publicKeyX;
    DualVariableGadget publicKeyY;

    // Calculate the new balance
    UnsafeAddGadget uncappedBalanceAfter;
    MinGadget balanceAfter;

    // Update User
    UpdateBalanceGadget updateBalance;
    UpdateAccountGadget updateAccount;

    DepositGadget(
        ProtoboardT& pb,
        const Constants& _constants,
        const VariableT& root,
        const std::string& prefix
    ) :
        GadgetT(pb, prefix),

        constants(_constants),

        // User state
        balanceBefore(pb, FMT(prefix, ".balanceBefore")),
        accountBefore(pb, FMT(prefix, ".accountBefore")),

        // Inputs
        accountID(pb, NUM_BITS_ACCOUNT, FMT(prefix, ".accountID")),
        tokenID(pb, NUM_BITS_TOKEN, FMT(prefix, ".tokenID")),
        amount(pb, NUM_BITS_AMOUNT, FMT(prefix, ".amount")),
        publicKeyX(pb, 256, FMT(prefix, ".publicKeyX")),
        publicKeyY(pb, 256, FMT(prefix, ".publicKeyY")),

        // Calculate the new balance
        // We can't let the deposit fail (it's onchain so it needs to be included),
        // and we do want to cap the balance to NUM_BITS_AMOUNT bits max, so cap the balance even
        // if it means that the user loses some tokens (NUM_BITS_AMOUNT bits should be more than enough).
        uncappedBalanceAfter(pb, balanceBefore.balance, amount.packed, FMT(prefix, ".uncappedBalanceAfter")),
        balanceAfter(pb, uncappedBalanceAfter.result(), constants.maxAmount, NUM_BITS_AMOUNT + 1, FMT(prefix, ".balanceAfter")),

        // Update User
        updateBalance(pb, accountBefore.balancesRoot, tokenID.bits,
                      {balanceBefore.balance, balanceBefore.tradingHistory},
                      {balanceAfter.result(), balanceBefore.tradingHistory},
                      FMT(prefix, ".updateBalance")),
        updateAccount(pb, root, accountID.bits,
                      {accountBefore.publicKey.x, accountBefore.publicKey.y, accountBefore.nonce, accountBefore.balancesRoot},
                      {publicKeyX.packed, publicKeyY.packed, accountBefore.nonce, updateBalance.result()},
                      FMT(prefix, ".updateAccount"))
    {

    }

    void generate_r1cs_witness(const Deposit& deposit)
    {
        // User state
        balanceBefore.generate_r1cs_witness(deposit.balanceUpdate.before);
        accountBefore.generate_r1cs_witness(deposit.accountUpdate.before);

        // Inputs
        accountID.generate_r1cs_witness(pb, deposit.accountUpdate.accountID);
        tokenID.generate_r1cs_witness(pb, deposit.balanceUpdate.tokenID);
        amount.generate_r1cs_witness(pb, deposit.amount);
        publicKeyX.generate_r1cs_witness(pb, deposit.accountUpdate.after.publicKey.x);
        publicKeyY.generate_r1cs_witness(pb, deposit.accountUpdate.after.publicKey.y);

        // Calculate the new balance
        uncappedBalanceAfter.generate_r1cs_witness();
        balanceAfter.generate_r1cs_witness();

        // Update User
        updateBalance.generate_r1cs_witness(deposit.balanceUpdate.proof);
        updateAccount.generate_r1cs_witness(deposit.accountUpdate.proof);
    }

    void generate_r1cs_constraints()
    {
        // Inputs
        accountID.generate_r1cs_constraints(true);
        tokenID.generate_r1cs_constraints(true);
        amount.generate_r1cs_constraints(true);
        publicKeyX.generate_r1cs_constraints(true);
        publicKeyY.generate_r1cs_constraints(true);

        // Calculate the new balance
        uncappedBalanceAfter.generate_r1cs_constraints();
        balanceAfter.generate_r1cs_constraints();

        // Update User
        updateBalance.generate_r1cs_constraints();
        updateAccount.generate_r1cs_constraints();
    }

    const std::vector<VariableArrayT> getOnchainData() const
    {
        return {accountID.bits,
                publicKeyX.bits, publicKeyY.bits,
                VariableArrayT(6, constants.zero), tokenID.bits,
                amount.bits};
    }

    const VariableT& getNewAccountsRoot() const
    {
        return updateAccount.result();
    }
};

}

#endif
