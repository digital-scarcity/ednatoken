/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "ednatoken.hpp"
#include <math.h>

void ednatoken::create( account_name issuer,
                    asset        maximum_supply )
{
    require_auth( _self );

    auto sym = maximum_supply.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( maximum_supply.is_valid(), "invalid supply");
    eosio_assert( maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable( _self, sym.name() );
    auto existing = statstable.find( sym.name() );
    eosio_assert( existing == statstable.end(), "stake with symbol already exists" );

    statstable.emplace( _self, [&]( auto& s ) {
       s.supply.symbol = maximum_supply.symbol;
       s.max_supply    = maximum_supply;
       s.issuer        = issuer;
    });
}


void ednatoken::issue( account_name to, asset quantity, string memo )
{
    eosio::print ("Issuing ");
    auto sym = quantity.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    auto sym_name = sym.name();
    stats statstable( _self, sym_name );
    auto existing = statstable.find( sym_name );
    eosio_assert( existing != statstable.end(), "stake with symbol does not exist, create stake before issue" );
    const auto& st = *existing;

    require_auth( st.issuer );
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must issue positive quantity" );

    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");
    

    statstable.modify( st, 0, [&]( auto& s ) {
       s.supply += quantity;
    });

    add_balance( st.issuer, quantity, st.issuer );

    if( to != st.issuer ) {
       SEND_INLINE_ACTION( *this, transfer, {st.issuer,N(active)}, {st.issuer, to, quantity, memo} );
    }
}

void ednatoken::transfer( account_name from,
                      account_name to,
                      asset        quantity,
                      string       memo )
{
    eosio_assert( from != to, "cannot transfer to self" );
    require_auth( from );
    eosio_assert( is_account( to ), "to account does not exist");
    auto sym = quantity.symbol.name();
    stats statstable( _self, sym );
    const auto& st = statstable.get( sym );

    require_recipient( from );
    require_recipient( to );

    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );


    sub_balance( from, quantity );
    add_balance( to, quantity, from );
}

void ednatoken::setoverflow (account_name _overflow) {
    config_table c_t (_self, _self);
    auto c_itr = c_t.find(0);
    if (c_itr == c_t.end()) {
        c_t.emplace (_self, [&](auto &c) {
            c.overflow = _overflow;
        });
    } else {
        c_t.modify(c_itr, _self, [&](auto &c) {
            c.overflow = _overflow;
        });
    }
}

void ednatoken::addbonus (account_name _sender, asset _bonus) {
    require_auth (_sender);
    config_table c_t (_self, _self);
    auto c_itr = c_t.find(0);
    if (c_itr == c_t.end()) {
        c_t.emplace (_self, [&](auto &c) {
            c.bonus = _bonus;
        });
    } else {
        c_t.modify(c_itr, _self, [&](auto &c) {
            c.bonus += _bonus;
        });
    }
    sub_balance(_sender, _bonus);
}

void ednatoken::process () {
    require_auth (_self);
    stake_table s_t (_self, _self);
    auto itr = s_t.begin();
    uint64_t total_shares = 0;
    asset total_stake {0, string_to_symbol(4, "EDNA")};
    while (itr != s_t.end()) {
        total_stake += itr->staked;
        if (itr->stake_period == WEEKLY) {
            total_shares += (100 * itr->staked.amount / 10000 / 100);   
        } else if (itr->stake_period == MONTHLY) {
            total_shares += 150 * itr->staked.amount / 100;
        } else if (itr->stake_period == QUARTERLY) {
            total_shares += 200 * itr->staked.amount  / 100;
        }
        itr++;
    }
    
    auto weekly_base = asset{BASE_WEEKLY, string_to_symbol(4, "EDNA")};
    print ("Weekly Base         : ", weekly_base, "\n");

    auto g_total_stake = total_stake;// * pow(10,6);

    auto supply = 1000000000;

    stats stat_t (_self, total_stake.symbol);
  
    print ("Total Stake         : ", total_stake, "\n");
    print ("G Total Stake       : ", g_total_stake, "\n");
    print ("Supplied Token      : ", supply, "\n");
    print ("Total Shares        : ", total_shares, "\n");
  
    auto perc_stakedx100 = g_total_stake / supply;// / 10 ^ 6 ;
    print ("Perc Stakedx100     : ", perc_stakedx100, "\n");

    auto base_payout = perc_stakedx100 * weekly_base.amount / 10000 ;
    print ("Base Payout         : ", base_payout, "\n");

    config_table c_t (_self, _self);
    auto c_itr = c_t.find(0);

    asset total_payout;
    if (c_itr->bonus.amount > 0) {
        total_payout = base_payout + (c_itr->bonus);// * pow(10,6));
        c_t.modify(c_itr, _self, [&](auto &c) {
            c.bonus = c.bonus * 0;  // multiple by zero to keep symbol
        });
    }
    else {
        total_payout = base_payout;
    }

    print ("Total Payout        : ", total_payout, "\n");
    
    auto unclaimed_tokens = weekly_base - total_payout;
    print ("Unclaimed Tokens    : ", unclaimed_tokens, "\n");
    // Send unclaimed tokens to the overflow account
    
    itr = s_t.begin();
    while (itr != s_t.end()) {
        print ("------  Reward  ---------------\n");
        print ("TO      : ", name{itr->stake_account}, "\n");
        asset payout;
        if (itr->stake_period == WEEKLY) {
            payout = (WEEK_MULTIPLIERX100 * itr->staked.amount / 100 ) / total_shares * total_payout / 10000;
        } else if (itr->stake_period == MONTHLY) {
            payout = (MONTH_MULTIPLIERX100 * itr->staked.amount / 100 ) / total_shares * total_payout / 10000;
        } else if (itr->stake_period == QUARTERLY) {
            payout = (QUARTER_MULTIPLIERX100 * itr->staked.amount / 100 ) / total_shares * total_payout / 10000;
        }
        print ("PAYOUT  : ", payout);
        print ("------  End Reward  ------------\n");
        itr++;
    }

}

void ednatoken::addstake (account_name _stake_account,
                            uint8_t      _stake_period,
                            asset       _staked )  {

    require_auth( _stake_account );
    eosio_assert( is_account( _stake_account ), "to account does not exist");
    auto sym = _staked.symbol.name();
    stats statstable( _self, sym );
    const auto& st = statstable.get( sym );

    eosio_assert( _staked.is_valid(), "invalid quantity" );
    eosio_assert( _staked.amount > 0, "must transfer positive quantity" );
    eosio_assert( _staked.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert(_stake_period >= 1 && _stake_period <= 3, "Invalid stake period.");

    sub_balance (_stake_account, _staked);
    
    stake_table s_t (_self, _self);
    s_t.emplace (_stake_account, [&](auto& s) {
        s.stake_id  = s_t.available_primary_key();
        s.stake_account = _stake_account;
        s.stake_period  = _stake_period;
        s.staked        = _staked;
    });
}

void ednatoken::sub_balance( account_name owner, asset value ) {
   accounts from_acnts( _self, owner );

   const auto& from = from_acnts.get( value.symbol.name(), "no balance object found" );
   eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );


   if( from.balance.amount == value.amount ) {
      from_acnts.erase( from );
   } else {
      from_acnts.modify( from, owner, [&]( auto& a ) {
          a.balance -= value;
      });
   }
}

void ednatoken::add_balance( account_name owner, asset value, account_name ram_payer )
{
   accounts to_acnts( _self, owner );
   auto to = to_acnts.find( value.symbol.name() );
   if( to == to_acnts.end() ) {
      to_acnts.emplace( ram_payer, [&]( auto& a ){
        a.balance = value;
       // a.staked = asset {0, value.symbol};
      });
   } else {
      to_acnts.modify( to, 0, [&]( auto& a ) {
        a.balance += value;
      });
   }
}

