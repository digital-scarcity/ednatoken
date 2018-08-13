/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <string>

using namespace eosio;
using std::string;

class ednatoken : public contract
{
  public:
    ednatoken(account_name self) : contract(self) {}

    // @abi action
    void create(account_name issuer,
                asset maximum_supply);

    // @abi action
    void issue(account_name to, asset quantity, string memo);

    // @abi action
    void addstake (account_name _stake_account,
                   uint8_t      _stake_period,
                   asset       _staked ) ;

    // @abi action
    void transfer(account_name from,
                  account_name to,
                  asset quantity,
                  string memo);

    // @abi action
    void setoverflow (account_name    _overflow);

    // @abi action 
    void addbonus ( account_name    _sender, 
                    asset           _bonus);

    // @abi action
    void process () ;
    
    inline asset get_supply(symbol_name sym) const;

    inline asset get_balance(account_name owner, symbol_name sym) const;

  private:

    const uint16_t  WEEK_MULTIPLIERX100 = 100;
    const uint16_t  MONTH_MULTIPLIERX100 = 150;
    const uint16_t  QUARTER_MULTIPLIERX100 = 200;
    const int64_t   BASE_WEEKLY = 20000000000;

    const uint8_t   WEEKLY = 1;
    const uint8_t   MONTHLY = 2;
    const uint8_t   QUARTERLY = 3;

    // @abi table configs i64
    struct config {
        uint64_t        config_id;
        asset           bonus;
        account_name    overflow;

        uint64_t    primary_key() const { return config_id; }
        EOSLIB_SERIALIZE (config, (config_id)(overflow));
    };

    typedef eosio::multi_index<N(configs), config> config_table;

    // @abi table accounts i64
    struct account
    {
        asset balance;
        uint64_t primary_key() const { return balance.symbol.name(); }

        EOSLIB_SERIALIZE (account, (balance));
    };

    // @abi table stakes i64
    struct stake_row {
        uint64_t        stake_id;
        account_name    stake_account;
        uint8_t         stake_period;
        asset           staked;
        uint32_t        stake_date;
        uint32_t        stake_due;

        uint64_t    primary_key () const { return stake_id; }
        EOSLIB_SERIALIZE (stake_row, (stake_id)(stake_account)(stake_period)(staked));
    };

    typedef eosio::multi_index<N(stakes), stake_row> stake_table;

    // @abi table stat i64
    struct currencystat
    {
        asset supply;
        asset max_supply;
        account_name issuer;

        uint64_t primary_key() const { return supply.symbol.name(); }

        EOSLIB_SERIALIZE (currencystat, (supply)(max_supply)(issuer));
    };

    typedef eosio::multi_index<N(accounts), account> accounts;
    typedef eosio::multi_index<N(stat), currencystat> stats;

    void sub_balance(account_name owner, asset value);
    void add_balance(account_name owner, asset value, account_name ram_payer);

    void sub_stake(account_name owner, asset value);
    void add_stake(account_name owner, asset value);

  public:
    struct transfer_args
    {
        account_name from;
        account_name to;
        asset quantity;
        string memo;
    };
};


asset ednatoken::get_supply(symbol_name sym) const
{
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);
    return st.supply;
}

asset ednatoken::get_balance(account_name owner, symbol_name sym) const
{
    accounts accountstable(_self, owner);
    const auto &ac = accountstable.get(sym);
    return ac.balance;
}


EOSIO_ABI( ednatoken, (setoverflow)(addbonus)(create)
                        (issue)(transfer)(addstake)(process))