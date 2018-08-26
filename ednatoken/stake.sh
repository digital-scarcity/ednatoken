

eosiocpp -g /edna/ednatoken/ednatoken.abi /edna/ednatoken/ednatoken.hpp && eosiocpp -o /edna/ednatoken/ednatoken.wast /edna/ednatoken/ednatoken.cpp

cleos create account eosio ednatoken2 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn

cleos create account eosio staker EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio staker1 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio staker2 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio staker3 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio staker4 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio staker5 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio overflow EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn

cleos set contract ednatoken2 /edna/ednatoken
cleos push action ednatoken2 create '["staker", "1300000000.0000 EDNA"]' -p ednatoken2
cleos push action ednatoken2 setoverflow '["overflow"]' -p ednatoken2

cleos push action ednatoken2 issue '["ednatoken2",  "200000000.0000 EDNA", "memo"]' -p staker

cleos push action ednatoken2 issue '["staker",  "200000000.0000 EDNA", "memo"]' -p staker
cleos push action ednatoken2 issue '["staker1", "200000000.0000 EDNA", "memo"]' -p staker
cleos push action ednatoken2 issue '["staker2", "200000000.0000 EDNA", "memo"]' -p staker
cleos push action ednatoken2 issue '["staker3", "200000000.0000 EDNA", "memo"]' -p staker
cleos push action ednatoken2 issue '["staker4", "200000000.0000 EDNA", "memo"]' -p staker

cleos push action ednatoken2 stake '["staker1", 1, "200000000.0000 EDNA"]' -p staker1
cleos push action ednatoken2 stake '["staker2", 1, "66663666.0000 EDNA"]' -p staker2
cleos push action ednatoken2 stake '["staker3", 1, "88882388.0000 EDNA"]' -p staker3
cleos push action ednatoken2 stake '["staker4", 1, "200000000.0000 EDNA"]' -p staker4

cleos push action ednatoken2 process '[0]' -p ednatoken2


#/opt/eosio/bin/data-dir/config.ini