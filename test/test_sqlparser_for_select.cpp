// Copyright (c) 2018 Baidu, Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <climits>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "parser.h"

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

namespace parser {

TEST(test_parser, case_option) {
    {
        parser::SqlParser parser;
        //select distict
        std::string sql_opt0 = "select distinct high_priority straight_join"
                                " sql_cache sql_calc_found_rows field_a";
        parser.parse(sql_opt0);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        ASSERT_TRUE(select_stmt->select_opt->distinct);
        ASSERT_TRUE(select_stmt->select_opt->sql_cache);
        ASSERT_TRUE(select_stmt->select_opt->calc_found_rows);
        ASSERT_TRUE(select_stmt->select_opt->straight_join);
        ASSERT_EQ(3, select_stmt->select_opt->priority);
        std::cout << select_stmt->to_string() << std::endl; 
    }
    //select
    {
        parser::SqlParser parser;
        //select distict
        std::string sql_opt1 = "select field_a ";
        parser.parse(sql_opt1);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        ASSERT_FALSE(select_stmt->select_opt->distinct);
        ASSERT_FALSE(select_stmt->select_opt->sql_cache);
        ASSERT_FALSE(select_stmt->select_opt->calc_found_rows);
        ASSERT_FALSE(select_stmt->select_opt->straight_join);
        ASSERT_EQ(0, select_stmt->select_opt->priority);
        std::cout << select_stmt->to_string() << std::endl; 
    }
    {
        parser::SqlParser parser;
        // select distictrow
        std::string sql_opt2 = "select all  sql_cache sql_calc_found_rows field_a ";
        parser.parse(sql_opt2);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        ASSERT_FALSE(select_stmt->select_opt->distinct);
        ASSERT_TRUE(select_stmt->select_opt->sql_cache);
        ASSERT_TRUE(select_stmt->select_opt->calc_found_rows);
        ASSERT_FALSE(select_stmt->select_opt->straight_join);
        ASSERT_EQ(0, select_stmt->select_opt->priority);
        std::cout << select_stmt->to_string() << std::endl; 
    }
    {
        parser::SqlParser parser;
        // select distictrow
        std::string sql_opt3 = "Select distinctrow  high_priority straight_join"
                                " sql_cache sql_calc_found_rows field_a";
        parser.parse(sql_opt3);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        ASSERT_TRUE(select_stmt->select_opt->distinct);
        ASSERT_TRUE(select_stmt->select_opt->sql_cache);
        ASSERT_TRUE(select_stmt->select_opt->calc_found_rows);
        ASSERT_TRUE(select_stmt->select_opt->straight_join);
        ASSERT_EQ(3, select_stmt->select_opt->priority);
        std::cout << select_stmt->to_string() << std::endl; 
    }
}

TEST(test_parser, case_field) {
    // test select_fields
    {
        parser::SqlParser parser;
        // select distictrow
        std::string sql_field0 = "select *";
        parser.parse(sql_field0);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        parser::SelectField* field = select_stmt->fields[0];
        ASSERT_TRUE(field->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(field->expr == nullptr); 
        ASSERT_TRUE(field->as_name.value == nullptr); 
        ASSERT_TRUE(field->wild_card != nullptr); 
        ASSERT_TRUE(field->wild_card->node_type == parser::NT_WILDCARD);
        ASSERT_TRUE(field->wild_card->db_name.value == nullptr);
        ASSERT_TRUE(field->wild_card->table_name.value == nullptr);
    }
    {
        parser::SqlParser parser;
        std::string sql_field1 = "select tablea.*";
        parser.parse(sql_field1);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        parser::SelectField* field = select_stmt->fields[0];
        ASSERT_TRUE(field->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(field->expr == nullptr); 
        ASSERT_TRUE(field->as_name.value == nullptr); 
        ASSERT_TRUE(field->wild_card != nullptr); 
        ASSERT_TRUE(field->wild_card->node_type == parser::NT_WILDCARD);
        ASSERT_TRUE(field->wild_card->db_name.value == nullptr);
        ASSERT_TRUE(std::string(field->wild_card->table_name.value) == "tablea");
    }
    {
        parser::SqlParser parser;
        std::string sql_field2 = "select db.tablea.*";
        parser.parse(sql_field2);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        parser::SelectField* field = select_stmt->fields[0];
        ASSERT_TRUE(field->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(field->expr == nullptr); 
        ASSERT_TRUE(field->as_name.value == nullptr); 
        ASSERT_TRUE(field->wild_card != nullptr); 
        ASSERT_TRUE(field->wild_card->node_type == parser::NT_WILDCARD);
        ASSERT_TRUE(!field->wild_card->db_name.empty());
        ASSERT_TRUE(!field->wild_card->table_name.empty());
        ASSERT_TRUE(std::string(field->wild_card->db_name.value) == "db");
        ASSERT_TRUE(std::string(field->wild_card->table_name.value) == "tablea");
    }
    {
        parser::SqlParser parser;
        std::string sql_field4 = "select field_a";
        parser.parse(sql_field4);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        parser::SelectField* field = select_stmt->fields[0];
        ASSERT_TRUE(field->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(field->expr != nullptr); 
        ASSERT_TRUE(field->as_name.value == nullptr); 
        ASSERT_TRUE(field->wild_card == nullptr);
    }
    {
        parser::SqlParser parser;
        std::string sql_field5 = "select field_a as alias";
        parser.parse(sql_field5);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        parser::SelectField* field = select_stmt->fields[0];
        ASSERT_TRUE(field->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(field->expr != nullptr); 
        ASSERT_TRUE(field->as_name.value != nullptr); 
        ASSERT_TRUE(std::string(field->as_name.value) == "alias");
        ASSERT_TRUE(field->wild_card == nullptr);
    }
    {
        parser::SqlParser parser;
        std::string sql_field6 = "select table_a.field_a as alias";
        parser.parse(sql_field6);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        parser::SelectField* field = select_stmt->fields[0];
        ASSERT_TRUE(field->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(field->expr != nullptr); 
        ASSERT_TRUE(field->as_name.value != nullptr); 
        ASSERT_TRUE(std::string(field->as_name.value) == "alias");
        ASSERT_TRUE(field->wild_card == nullptr);
    }
    {
        parser::SqlParser parser;
        std::string sql_field7 = "select db.table_a.field_a as alias";
        parser.parse(sql_field7);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        parser::SelectField* field = select_stmt->fields[0];
        ASSERT_TRUE(field->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(field->expr != nullptr); 
        ASSERT_TRUE(field->as_name.value != nullptr); 
        ASSERT_TRUE(std::string(field->as_name.value) == "alias");
        ASSERT_TRUE(field->wild_card == nullptr);
    }
    {
        parser::SqlParser parser;
        std::string sql_field8 = "select {OJ field_a + 3} as alias";
        parser.parse(sql_field8);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        parser::SelectField* field = select_stmt->fields[0];
        ASSERT_TRUE(field->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(field->expr != nullptr); 
        ASSERT_TRUE(field->as_name.value != nullptr); 
        ASSERT_TRUE(std::string(field->as_name.value) == "alias");
        ASSERT_TRUE(field->wild_card == nullptr);
    }
    {
        parser::SqlParser parser;
        std::string sql_field9 = "select *, tablea.*, field_a, field_a as alias_1,"
                                " {OJ field_a + 3} as alias_2";
        parser.parse(sql_field9);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->fields[0]->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(select_stmt->fields[0]->expr == nullptr); 
        ASSERT_TRUE(select_stmt->fields[0]->as_name.value == nullptr); 
        ASSERT_TRUE(select_stmt->fields[0]->wild_card != nullptr); 
        ASSERT_TRUE(select_stmt->fields[0]->wild_card->node_type == parser::NT_WILDCARD);
        ASSERT_TRUE(select_stmt->fields[0]->wild_card->db_name.value == nullptr);
        ASSERT_TRUE(select_stmt->fields[0]->wild_card->table_name.value == nullptr);
        ASSERT_TRUE(select_stmt->fields[1]->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(select_stmt->fields[1]->expr == nullptr); 
        ASSERT_TRUE(select_stmt->fields[1]->as_name.value == nullptr); 
        ASSERT_TRUE(select_stmt->fields[1]->wild_card != nullptr); 
        ASSERT_TRUE(select_stmt->fields[1]->wild_card->node_type == parser::NT_WILDCARD);
        ASSERT_TRUE(select_stmt->fields[1]->wild_card->db_name.value == nullptr);
        ASSERT_TRUE(std::string(select_stmt->fields[1]->wild_card->table_name.value) == "tablea");
        ASSERT_TRUE(select_stmt->fields[2]->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(select_stmt->fields[2]->expr != nullptr); 
        ASSERT_TRUE(select_stmt->fields[2]->as_name.value == nullptr); 
        ASSERT_TRUE(select_stmt->fields[2]->wild_card == nullptr);
        ASSERT_TRUE(select_stmt->fields[3]->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(select_stmt->fields[3]->expr != nullptr); 
        ASSERT_TRUE(std::string(select_stmt->fields[3]->as_name.value) == "alias_1"); 
        ASSERT_TRUE(select_stmt->fields[3]->wild_card == nullptr);
        ASSERT_TRUE(select_stmt->fields[4]->node_type == parser::NT_SELECT_FEILD);
        ASSERT_TRUE(select_stmt->fields[4]->expr != nullptr); 
        ASSERT_TRUE(std::string(select_stmt->fields[4]->as_name.value) == "alias_2"); 
        ASSERT_TRUE(select_stmt->fields[4]->wild_card == nullptr);
    }
}

TEST(test_parser, case_orderby) {
    // test select_orderby
    {
        parser::SqlParser parser;
        std::string sql_orderby0 = "select *, tablea.*, field_a,"
                                    " field_a as alias_1, {OJ field_a + 3} as alias_2 ";
        parser.parse(sql_orderby0);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
    } 
    // test select_orderby
    {
        parser::SqlParser parser;
        std::string sql_orderby1 = "select *, tablea.*, field_a, field_a as alias_1,"
                                    " {OJ field_a + 3} as alias_2 order by field_a";
        parser.parse(sql_orderby1);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(1, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item = order_by->items[0];
        ASSERT_TRUE(by_item->expr != nullptr);
        ASSERT_TRUE(by_item->is_desc == false);
    } 
    {
        parser::SqlParser parser;
        std::string sql_orderby2 = "select *, tablea.*, field_a, field_a as alias_1,"
                                    " {OJ field_a + 3} as alias_2 order by field_a asc";
        parser.parse(sql_orderby2);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(1, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item = order_by->items[0];
        ASSERT_TRUE(by_item->expr != nullptr);
        ASSERT_TRUE(by_item->is_desc == false);
    } 
    {
        parser::SqlParser parser;
        std::string sql_orderby3 = "select *, tablea.*, field_a, field_a as alias_1,"
                                    " {OJ field_a + 3} as alias_2 order by field_a desc";
        parser.parse(sql_orderby3);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(1, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item = order_by->items[0];
        ASSERT_TRUE(by_item->expr != nullptr);
        ASSERT_TRUE(by_item->is_desc == true);
    } 
    {
        parser::SqlParser parser;
        std::string sql_orderby4 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 order by field_a asc, field_b desc";
        parser.parse(sql_orderby4);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
    } 
}
TEST(test_parser, case_limit) {
    {
        parser::SqlParser parser;
        std::string sql_limit0 = "select *, tablea.*, field_a, field_a as alias_1, {OJ field_a + 3}"
                                " as alias_2 order by field_a asc, field_b desc";
        parser.parse(sql_limit0);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
        ASSERT_TRUE(select_stmt->limit == nullptr);
    }
    {
        parser::SqlParser parser;
        std::string sql_limit1 = "select *, tablea.*, field_a, field_a as alias_1, {OJ field_a + 3}"
                                " as alias_2 order by field_a asc, field_b desc limit 10";
        parser.parse(sql_limit1);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
        ASSERT_TRUE(select_stmt->limit != nullptr);
        parser::LimitClause* limit = select_stmt->limit;
        ASSERT_EQ(10, limit->count);
        ASSERT_EQ(0, limit->offset);
    }
    {
        parser::SqlParser parser;
        std::string sql_limit2 = "select *, tablea.*, field_a, field_a as alias_1,"
                " {OJ field_a + 3} as alias_2 order by field_a asc, field_b desc limit 10, 100";
        parser.parse(sql_limit2);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
        ASSERT_TRUE(select_stmt->limit != nullptr);
        parser::LimitClause* limit = select_stmt->limit;
        ASSERT_EQ(100, limit->count);
        ASSERT_EQ(10, limit->offset);
    }
}

TEST(test_parser, case_lock) {
    //test select_lock
    {
        parser::SqlParser parser;
        std::string sql_lock0 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 order by field_a asc, field_b desc limit 10, 100";
        parser.parse(sql_lock0);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
        ASSERT_TRUE(select_stmt->limit != nullptr);
        parser::LimitClause* limit = select_stmt->limit;
        ASSERT_EQ(100, limit->count);
        ASSERT_EQ(10, limit->offset);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
    }
    {
        parser::SqlParser parser;
        std::string sql_lock1 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 order by field_a asc,"
            " field_b desc limit 10, 100 for update";
        parser.parse(sql_lock1);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
        ASSERT_TRUE(select_stmt->limit != nullptr);
        parser::LimitClause* limit = select_stmt->limit;
        ASSERT_EQ(100, limit->count);
        ASSERT_EQ(10, limit->offset);
        ASSERT_TRUE(select_stmt->lock == parser::SL_FOR_UPDATE);
    }
    {
        parser::SqlParser parser;
        std::string sql_lock2 = "select *, tablea.*, field_a, field_a as alias_1,"
                                " {OJ field_a + 3} as alias_2 order by field_a asc,"
                                " field_b desc limit 10, 100 lock in share mode";
        parser.parse(sql_lock2);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
        ASSERT_TRUE(select_stmt->limit != nullptr);
        parser::LimitClause* limit = select_stmt->limit;
        ASSERT_EQ(100, limit->count);
        ASSERT_EQ(10, limit->offset);
        ASSERT_TRUE(select_stmt->lock == parser::SL_IN_SHARE);
    }
}

TEST(test_parser, case_dual) {
    //test select_from_dual
    {
        parser::SqlParser parser;
        std::string sql_dual = "select *, tablea.*, field_a, field_a as alias_1,"
                                " {OJ field_a + 3} as alias_2 from dual"
                                " limit 10, 100 lock in share mode";
        parser.parse(sql_dual);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->limit != nullptr);
        parser::LimitClause* limit = select_stmt->limit;
        ASSERT_EQ(100, limit->count);
        ASSERT_EQ(10, limit->offset);
        ASSERT_TRUE(select_stmt->lock == parser::SL_IN_SHARE);
    }
}
TEST(test_parser, case_where) {
    // test selec_from where
    {
        parser::SqlParser parser;
        std::string sql_where = "select *, tablea.*, field_a, field_a as alias_1, {OJ field_a + 3}"
                                " as alias_2 from table_a where expr1 > 10 and "
                                "expr2 = 3 or expr3 = 4 order by field_a asc,"
                                " field_b desc limit 10, 100 lock in share mode";
        parser.parse(sql_where);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
        ASSERT_TRUE(select_stmt->limit != nullptr);
        parser::LimitClause* limit = select_stmt->limit;
        ASSERT_EQ(100, limit->count);
        ASSERT_EQ(10, limit->offset);
        ASSERT_TRUE(select_stmt->lock == parser::SL_IN_SHARE);
        ASSERT_TRUE(select_stmt->where != nullptr);
    }
}
TEST(test_parser, case_group) {
    {
        parser::SqlParser parser;
        //test select_group 
        std::string sql_group = "select *, tablea.*, field_a, field_a as alias_1, {OJ field_a + 3}"
                                " as alias_2 from table_a where expr1 > 10 and expr2 = 3 "
                                "or expr3 = 4 group by field_a + 4, field_b order by field_a asc,"
                                " field_b desc limit 10, 100 lock in share mode";
        parser.parse(sql_group);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
        ASSERT_TRUE(select_stmt->limit != nullptr);
        parser::LimitClause* limit = select_stmt->limit;
        ASSERT_EQ(100, limit->count);
        ASSERT_EQ(10, limit->offset);
        ASSERT_TRUE(select_stmt->lock == parser::SL_IN_SHARE);
        ASSERT_TRUE(select_stmt->where != nullptr);
        ASSERT_TRUE(select_stmt->group != nullptr);
        GroupByClause* group = select_stmt->group;
        ASSERT_EQ(2, group->items.size());
        ASSERT_TRUE(select_stmt->having == nullptr);
    }
}
TEST(test_parser, case_having) {
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_having = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from table_a where expr1 > 10 and expr2 = 3"
            " or expr3 = 4 group by field_a + 4, field_b having count(field_a) > 100"
            " and avg(field_b  - 100) > 10 order by field_a asc, field_b desc"
            " limit 10, 100 lock in share mode";
        parser.parse(sql_having);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order != nullptr);
        parser::OrderByClause* order_by = select_stmt->order;
        ASSERT_EQ(2, order_by->items.size());
        ASSERT_TRUE(order_by->node_type == parser::NT_ORDER_BY);
        parser::ByItem* by_item_0 = order_by->items[0];
        ASSERT_TRUE(by_item_0->expr != nullptr);
        ASSERT_TRUE(by_item_0->is_desc == false);
        parser::ByItem* by_item_1 = order_by->items[1];
        ASSERT_TRUE(by_item_1->expr != nullptr);
        ASSERT_TRUE(by_item_1->is_desc == true);
        ASSERT_TRUE(select_stmt->limit != nullptr);
        parser::LimitClause* limit = select_stmt->limit;
        ASSERT_EQ(100, limit->count);
        ASSERT_EQ(10, limit->offset);
        ASSERT_TRUE(select_stmt->lock == parser::SL_IN_SHARE);
        ASSERT_TRUE(select_stmt->where != nullptr);
        ASSERT_TRUE(select_stmt->group != nullptr);
        GroupByClause* group = select_stmt->group;
        ASSERT_EQ(2, group->items.size());
        ASSERT_TRUE(select_stmt->having != nullptr);
    }
}
TEST(test_parser, case_table_refs) {
// test select_from
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from0 = "select *, tablea.*, field_a, field_a as alias_1,"
                " {OJ field_a + 3} as alias_2 from table_a";
        parser.parse(sql_from0);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::TableSource));
        parser::TableSource* table_source = (parser::TableSource*)select_stmt->table_refs;
        ASSERT_EQ(0, table_source->index_hints.size());
        ASSERT_TRUE(table_source->as_name.empty());
        ASSERT_TRUE(table_source->table_name != nullptr);
        parser::TableName* table_name = table_source->table_name;
        ASSERT_TRUE(table_name->db.empty());
        ASSERT_TRUE(std::string(table_name->table.value) == "table_a");
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from1 = "select *, tablea.*, field_a, field_a as alias_1,"
                                " {OJ field_a + 3} as alias_2 from db.table_a";
        parser.parse(sql_from1);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::TableSource));
        parser::TableSource* table_source = (parser::TableSource*)select_stmt->table_refs;
        ASSERT_EQ(0, table_source->index_hints.size());
        ASSERT_TRUE(table_source->as_name.empty());
        ASSERT_TRUE(table_source->table_name != nullptr);
        parser::TableName* table_name = table_source->table_name;
        ASSERT_TRUE(std::string(table_name->db.value) == "db");
        ASSERT_TRUE(std::string(table_name->table.value) == "table_a");
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from2 = "select *, tablea.*, field_a, field_a as alias_1,"
                " {OJ field_a + 3} as alias_2 from db.table_a as table_alias";
        parser.parse(sql_from2);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::TableSource));
        parser::TableSource* table_source = (parser::TableSource*)select_stmt->table_refs;
        ASSERT_EQ(0, table_source->index_hints.size());
        ASSERT_TRUE(std::string(table_source->as_name.value) == "table_alias");
        ASSERT_TRUE(table_source->table_name != nullptr);
        parser::TableName* table_name = table_source->table_name;
        ASSERT_TRUE(std::string(table_name->db.value) == "db");
        ASSERT_TRUE(std::string(table_name->table.value) == "table_a");
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from3 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a as table_alias use key ()";
        parser.parse(sql_from3);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::TableSource));
        parser::TableSource* table_source = (parser::TableSource*)select_stmt->table_refs;
        ASSERT_EQ(1, table_source->index_hints.size());
        parser::IndexHint* index_hint = table_source->index_hints[0];
        ASSERT_EQ(parser::IHT_HINT_USE, index_hint->hint_type);
        ASSERT_EQ(parser::IHS_HINT_SCAN, index_hint->hint_scope);
        ASSERT_EQ(0, index_hint->index_name_list.size());
        ASSERT_TRUE(std::string(table_source->as_name.value) == "table_alias");
        ASSERT_TRUE(table_source->table_name != nullptr);
        parser::TableName* table_name = table_source->table_name;
        ASSERT_TRUE(std::string(table_name->db.value) == "db");
        ASSERT_TRUE(std::string(table_name->table.value) == "table_a");
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from4 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a as table_alias"
            " use key for join (primary_key)";
        parser.parse(sql_from4);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::TableSource));
        parser::TableSource* table_source = (parser::TableSource*)select_stmt->table_refs;
        ASSERT_EQ(1, table_source->index_hints.size());
        parser::IndexHint* index_hint = table_source->index_hints[0];
        ASSERT_EQ(parser::IHT_HINT_USE, index_hint->hint_type);
        ASSERT_EQ(parser::IHS_HINT_JOIN, index_hint->hint_scope);
        ASSERT_EQ(1, index_hint->index_name_list.size());
        ASSERT_EQ(std::string(index_hint->index_name_list[0].value), "primary_key");
        ASSERT_TRUE(std::string(table_source->as_name.value) == "table_alias");
        ASSERT_TRUE(table_source->table_name != nullptr);
        parser::TableName* table_name = table_source->table_name;
        ASSERT_TRUE(std::string(table_name->db.value) == "db");
        ASSERT_TRUE(std::string(table_name->table.value) == "table_a");
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from5 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a"
            " ignore index for order by (uniq_key, primary)";
        parser.parse(sql_from5);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::TableSource));
        parser::TableSource* table_source = (parser::TableSource*)select_stmt->table_refs;
        ASSERT_EQ(1, table_source->index_hints.size());
        parser::IndexHint* index_hint = table_source->index_hints[0];
        ASSERT_EQ(parser::IHT_HINT_IGNORE, index_hint->hint_type);
        ASSERT_EQ(parser::IHS_HINT_ORDER_BY, index_hint->hint_scope);
        ASSERT_EQ(2, index_hint->index_name_list.size());
        ASSERT_EQ(std::string(index_hint->index_name_list[0].value), "uniq_key");
        ASSERT_EQ(std::string(index_hint->index_name_list[1].value), "primary");
        ASSERT_TRUE(table_source->as_name.value == nullptr);
        ASSERT_TRUE(table_source->table_name != nullptr);
        parser::TableName* table_name = table_source->table_name;
        ASSERT_TRUE(std::string(table_name->db.value) == "db");
        ASSERT_TRUE(std::string(table_name->table.value) == "table_a");
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from6 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a as table_alias"
            " force key (uniq_key, primary)";
        parser.parse(sql_from6);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::TableSource));
        parser::TableSource* table_source = (parser::TableSource*)select_stmt->table_refs;
        ASSERT_EQ(1, table_source->index_hints.size());
        parser::IndexHint* index_hint = table_source->index_hints[0];
        ASSERT_EQ(parser::IHT_HINT_FORCE, index_hint->hint_type);
        ASSERT_EQ(parser::IHS_HINT_SCAN, index_hint->hint_scope);
        ASSERT_EQ(2, index_hint->index_name_list.size());
        ASSERT_EQ(std::string(index_hint->index_name_list[0].value), "uniq_key");
        ASSERT_EQ(std::string(index_hint->index_name_list[1].value), "primary");
        ASSERT_TRUE(std::string(table_source->as_name.value) == "table_alias");
        ASSERT_TRUE(table_source->table_name != nullptr);
        parser::TableName* table_name = table_source->table_name;
        ASSERT_TRUE(std::string(table_name->db.value) == "db");
        ASSERT_TRUE(std::string(table_name->table.value) == "table_a");
    }

    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from7 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a join table_b " ;
        parser.parse(sql_from7);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_EQ(nullptr, join_node->expr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from8 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a cross join table_b"; 
        parser.parse(sql_from8);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_EQ(nullptr, join_node->expr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from9 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a inner join table_b" ;
        parser.parse(sql_from9);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_EQ(nullptr, join_node->expr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from10 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a join table_b on id1 = id2"; 
        parser.parse(sql_from10);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        //test select_having
        std::string sql_from11 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a cross join table_b on id1 = id2"; 
        parser.parse(sql_from11);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from12 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a inner join table_b"
            " on id1 = id2 and id1 = id3";
        //test select_having
        parser.parse(sql_from12);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from13 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a join table_b using (id1)"; 
        parser.parse(sql_from13);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(1, join_node->using_col.size());
        parser::ColumnName* column_name = join_node->using_col[0];
        ASSERT_TRUE(column_name->db.value == nullptr);
        ASSERT_TRUE(column_name->table.value == nullptr);
        ASSERT_TRUE(std::string(column_name->name.value) == "id1");
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from14 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a cross join table_b using (id1, id2)"; 
        parser.parse(sql_from14);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(2, join_node->using_col.size());
        parser::ColumnName* column_name0 = join_node->using_col[0];
        ASSERT_TRUE(column_name0->db.value == nullptr);
        ASSERT_TRUE(column_name0->table.value == nullptr);
        ASSERT_TRUE(std::string(column_name0->name.value) == "id1");
        parser::ColumnName* column_name1 = join_node->using_col[1];
        ASSERT_TRUE(column_name1->db.value == nullptr);
        ASSERT_TRUE(column_name1->table.value == nullptr);
        ASSERT_TRUE(std::string(column_name1->name.value) == "id2");
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }

    {
        parser::SqlParser parser;
        std::string sql_from15 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a left outer join table_b on id1 = id2"; 
        //test select_having
        parser.parse(sql_from15);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from16 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a right outer join table_b on id2 = id1" ;
        //test select_having
        parser.parse(sql_from16);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_RIGHT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from17 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a left join table_b on id1 = id2";
        //test select_having
        parser.parse(sql_from17);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from18 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a right join table_b on id1 = id2"; 
        //test select_having
        parser.parse(sql_from18);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_RIGHT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from19 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a left outer join table_b using (id1) " ;
        parser.parse(sql_from19);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(1, join_node->using_col.size());
        parser::ColumnName* column_name = join_node->using_col[0];
        ASSERT_TRUE(column_name->db.value == nullptr);
        ASSERT_TRUE(column_name->table.value == nullptr);
        ASSERT_TRUE(std::string(column_name->name.value) == "id1");
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from20 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a right outer join table_b using (id1)" ;
        parser.parse(sql_from20);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_RIGHT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(1, join_node->using_col.size());
        parser::ColumnName* column_name = join_node->using_col[0];
        ASSERT_TRUE(column_name->db.value == nullptr);
        ASSERT_TRUE(column_name->table.value == nullptr);
        ASSERT_TRUE(std::string(column_name->name.value) == "id1");
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from21 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a left join table_b using (id1)"; 
        parser.parse(sql_from21);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(1, join_node->using_col.size());
        parser::ColumnName* column_name = join_node->using_col[0];
        ASSERT_TRUE(column_name->db.value == nullptr);
        ASSERT_TRUE(column_name->table.value == nullptr);
        ASSERT_TRUE(std::string(column_name->name.value) == "id1");
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from22 = "select *, tablea.*, field_a, field_a as alias_1,"
            " {OJ field_a + 3} as alias_2 from db.table_a right join table_b using (id2)"; 
        parser.parse(sql_from22);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(5, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_RIGHT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(1, join_node->using_col.size());
        parser::ColumnName* column_name = join_node->using_col[0];
        ASSERT_TRUE(column_name->db.value == nullptr);
        ASSERT_TRUE(column_name->table.value == nullptr);
        ASSERT_TRUE(std::string(column_name->name.value) == "id2");
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(right->table_name->db.value == nullptr);
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }

    {
        parser::SqlParser parser;
        std::string sql_from29 = "select * from db.table_a straight_join db.table_b";
        parser.parse(sql_from29);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_TRUE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(std::string(right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from30 = "select * from db.table_a straight_join"
            " db.table_b on id1 = id2 and id1 = id3";
        parser.parse(sql_from30);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_TRUE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(std::string(right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    
    {
        parser::SqlParser parser;
        std::string sql_from31 = "select * from db.table_a natural join db.table_b";
        parser.parse(sql_from31);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_TRUE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(std::string(right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from32 = "select * from db.table_a natural inner join db.table_b";
        parser.parse(sql_from32);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_TRUE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(std::string(right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    
    {
        parser::SqlParser parser;
        std::string sql_from33 = "select * from db.table_a natural left join db.table_b";
        parser.parse(sql_from33);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, join_node->join_type);
        ASSERT_TRUE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(std::string(right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from34 = "select * from db.table_a natural right join db.table_b";
        parser.parse(sql_from34);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_RIGHT_JOIN, join_node->join_type);
        ASSERT_TRUE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);
        
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(std::string(right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from35 = "select * from db.table_a left join db.table_b "
            "on table_a.id = table_b.id right join db.table_c on table_b.id1 = tablec.id1";
        parser.parse(sql_from35);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_RIGHT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::JoinNode));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(std::string(right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_c");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);

        parser::JoinNode* left_join_node = (parser::JoinNode*)join_node->left;
        std::cout << "left_join_node: " << left_join_node->to_string() << std::endl;
        ASSERT_EQ(parser::NT_JOIN, left_join_node->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, left_join_node->join_type);
        ASSERT_FALSE(left_join_node->is_natural);
        ASSERT_FALSE(left_join_node->is_straight);
        ASSERT_EQ(0, left_join_node->using_col.size());
        ASSERT_TRUE(left_join_node->expr != nullptr);
        ASSERT_TRUE(left_join_node->left != nullptr);
        ASSERT_TRUE(left_join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(left_join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(left_join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left_join_node_right = (parser::TableSource*)left_join_node->right;
        ASSERT_TRUE(left_join_node_right->table_name != nullptr);
        ASSERT_TRUE(std::string(left_join_node_right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left_join_node_right->table_name->table.value) == "table_b");
        ASSERT_TRUE(left_join_node_right->as_name.value == nullptr);
        ASSERT_TRUE(left_join_node_right->index_hints.size() == 0);
        parser::TableSource* left_join_node_left = (parser::TableSource*)left_join_node->left;
        ASSERT_TRUE(left_join_node_left->table_name != nullptr);
        ASSERT_TRUE(std::string(left_join_node_left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left_join_node_left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left_join_node_left->as_name.value == nullptr);
        ASSERT_TRUE(left_join_node_left->index_hints.size() == 0);
    }

    {
        parser::SqlParser parser;
        std::string sql_from36 = "select * from db.table_a left join "
            "(db.table_b left join db.table_c on table_a.id = table_b.id) "
            "on table_b.id1 = tablec.id1";
        parser.parse(sql_from36);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr != nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::JoinNode));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);

        parser::JoinNode* right_join_node = (parser::JoinNode*)join_node->right;
        std::cout << "right_join_node: " << right_join_node->to_string() << std::endl;
        ASSERT_EQ(parser::NT_JOIN, right_join_node->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, right_join_node->join_type);
        ASSERT_FALSE(right_join_node->is_natural);
        ASSERT_FALSE(right_join_node->is_straight);
        ASSERT_EQ(0, right_join_node->using_col.size());
        ASSERT_TRUE(right_join_node->expr != nullptr);
        ASSERT_TRUE(right_join_node->left != nullptr);
        ASSERT_TRUE(right_join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(right_join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(right_join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* right_join_node_right = (parser::TableSource*)right_join_node->right;
        ASSERT_TRUE(right_join_node_right->table_name != nullptr);
        ASSERT_TRUE(std::string(right_join_node_right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right_join_node_right->table_name->table.value) == "table_c");
        ASSERT_TRUE(right_join_node_right->as_name.value == nullptr);
        ASSERT_TRUE(right_join_node_right->index_hints.size() == 0);
        parser::TableSource* right_join_node_left = (parser::TableSource*)right_join_node->left;
        ASSERT_TRUE(right_join_node_left->table_name != nullptr);
        ASSERT_TRUE(std::string(right_join_node_left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right_join_node_left->table_name->table.value) == "table_b");
        ASSERT_TRUE(right_join_node_left->as_name.value == nullptr);
        ASSERT_TRUE(right_join_node_left->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from37 = "select * from db.table_a , db.table_b";
        parser.parse(sql_from37);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* left = (parser::TableSource*)join_node->left;
        ASSERT_TRUE(left->table_name != nullptr);
        ASSERT_TRUE(std::string(left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left->as_name.value == nullptr);
        ASSERT_TRUE(left->index_hints.size() == 0);

        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(std::string(right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_b");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);
    }
    {
        parser::SqlParser parser;
        std::string sql_from38 = "select * from db.table_a, db.table_b, db.table_c";
        parser.parse(sql_from38);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::JoinNode));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::TableSource));
        parser::TableSource* right = (parser::TableSource*)join_node->right;
        ASSERT_TRUE(right->table_name != nullptr);
        ASSERT_TRUE(std::string(right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right->table_name->table.value) == "table_c");
        ASSERT_TRUE(right->as_name.value == nullptr);
        ASSERT_TRUE(right->index_hints.size() == 0);

        parser::JoinNode* left = (parser::JoinNode*)join_node->left;
        ASSERT_EQ(parser::NT_JOIN, left->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, left->join_type);
        ASSERT_FALSE(left->is_natural);
        ASSERT_FALSE(left->is_straight);
        ASSERT_EQ(0, left->using_col.size());
        ASSERT_TRUE(left->expr == nullptr);
        ASSERT_TRUE(left->left != nullptr);
        ASSERT_TRUE(left->right != nullptr);
        ASSERT_TRUE(typeid(*(left->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(left->right)) == typeid(parser::TableSource));
        parser::TableSource* left_right = (parser::TableSource*)left->right;
        ASSERT_TRUE(left_right->table_name != nullptr);
        ASSERT_TRUE(std::string(left_right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left_right->table_name->table.value) == "table_b");
        ASSERT_TRUE(left_right->as_name.value == nullptr);
        ASSERT_TRUE(left_right->index_hints.size() == 0);
        
        parser::TableSource* left_left = (parser::TableSource*)left->left;
        ASSERT_TRUE(left_left->table_name != nullptr);
        ASSERT_TRUE(std::string(left_left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left_left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left_left->as_name.value == nullptr);
        ASSERT_TRUE(left_left->index_hints.size() == 0);

    }
    {
        parser::SqlParser parser;
        std::string sql_from39 = "select * from db.table_a left join "
                "(db.table_b left join db.table_c on table_a.id = table_b.id)"
                " on table_b.id1 = tablec.id1, "
                "(db.table_d inner join db.table_e on table_d.id = table_e.id)";
        parser.parse(sql_from39);
        if (parser.error != parser::SUCC) {
            std::cout <<  parser.result.size() << "error:" << parser.syntax_err_str << std::endl;
            return;
        }
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(parser::SelectStmt));
        parser::SelectStmt* select_stmt = (parser::SelectStmt*)parser.result[0];
        std::cout << select_stmt->to_string() << std::endl;
        ASSERT_EQ(1, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->order == nullptr);
        ASSERT_TRUE(select_stmt->limit == nullptr);
        ASSERT_TRUE(select_stmt->lock == parser::SL_NONE);
        ASSERT_TRUE(select_stmt->where == nullptr);
        ASSERT_TRUE(select_stmt->group == nullptr);
        ASSERT_TRUE(select_stmt->having == nullptr);
        ASSERT_TRUE(select_stmt->table_refs != nullptr);
        ASSERT_TRUE(typeid(*(select_stmt->table_refs)) == typeid(parser::JoinNode));
        parser::JoinNode* join_node = (parser::JoinNode*)select_stmt->table_refs;
        ASSERT_EQ(parser::NT_JOIN, join_node->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, join_node->join_type);
        ASSERT_FALSE(join_node->is_natural);
        ASSERT_FALSE(join_node->is_straight);
        ASSERT_EQ(0, join_node->using_col.size());
        ASSERT_TRUE(join_node->expr == nullptr);
        ASSERT_TRUE(join_node->left != nullptr);
        ASSERT_TRUE(join_node->right != nullptr);
        ASSERT_TRUE(typeid(*(join_node->left)) == typeid(parser::JoinNode));
        ASSERT_TRUE(typeid(*(join_node->right)) == typeid(parser::JoinNode));
        parser::JoinNode* right = (parser::JoinNode*)join_node->right;
        ASSERT_EQ(parser::NT_JOIN, right->node_type);
        ASSERT_EQ(parser::JT_INNER_JOIN, right->join_type);
        ASSERT_TRUE(typeid(*(right->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(right->right)) == typeid(parser::TableSource));
        parser::TableSource* right_left = (parser::TableSource*)right->left;
        parser::TableSource* right_right = (parser::TableSource*)right->right;
        ASSERT_TRUE(right_left->table_name != nullptr);
        ASSERT_TRUE(std::string(right_left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right_left->table_name->table.value) == "table_d");
        ASSERT_TRUE(right_left->as_name.value == nullptr);
        ASSERT_TRUE(right_left->index_hints.size() == 0);

        ASSERT_TRUE(right_right->table_name != nullptr);
        ASSERT_TRUE(std::string(right_right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(right_right->table_name->table.value) == "table_e");
        ASSERT_TRUE(right_right->as_name.value == nullptr);
        ASSERT_TRUE(right_right->index_hints.size() == 0);

        parser::JoinNode* left = (parser::JoinNode*)join_node->left;
        ASSERT_EQ(parser::NT_JOIN, left->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, left->join_type);
        ASSERT_FALSE(left->is_natural);
        ASSERT_FALSE(left->is_straight);
        ASSERT_EQ(0, left->using_col.size());
        ASSERT_TRUE(left->expr != nullptr);
        ASSERT_TRUE(left->left != nullptr);
        ASSERT_TRUE(left->right != nullptr);
        ASSERT_TRUE(typeid(*(left->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(left->right)) == typeid(parser::JoinNode));
        parser::TableSource* left_left = (parser::TableSource*)left->left;
        ASSERT_TRUE(left_left->table_name != nullptr);
        ASSERT_TRUE(std::string(left_left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left_left->table_name->table.value) == "table_a");
        ASSERT_TRUE(left_left->as_name.value == nullptr);
        ASSERT_TRUE(left_left->index_hints.size() == 0);
        
        parser::JoinNode* left_right = (parser::JoinNode*)left->right;
        ASSERT_EQ(parser::NT_JOIN, left_right->node_type);
        ASSERT_EQ(parser::JT_LEFT_JOIN, left_right->join_type);
        ASSERT_FALSE(left_right->is_natural);
        ASSERT_FALSE(left_right->is_straight);
        ASSERT_EQ(0, left_right->using_col.size());
        ASSERT_TRUE(left_right->expr != nullptr);
        ASSERT_TRUE(left_right->left != nullptr);
        ASSERT_TRUE(left_right->right != nullptr);
        ASSERT_TRUE(typeid(*(left_right->left)) == typeid(parser::TableSource));
        ASSERT_TRUE(typeid(*(left_right->right)) == typeid(parser::TableSource));
        parser::TableSource* left_right_left = (parser::TableSource*)left_right->left;
        ASSERT_TRUE(left_right_left->table_name != nullptr);
        ASSERT_TRUE(std::string(left_right_left->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left_right_left->table_name->table.value) == "table_b");
    
        parser::TableSource* left_right_right = (parser::TableSource*)left_right->right;
        ASSERT_TRUE(left_right_right->table_name != nullptr);
        ASSERT_TRUE(std::string(left_right_right->table_name->db.value) == "db");
        ASSERT_TRUE(std::string(left_right_right->table_name->table.value) == "table_c");
    }
}
}  // namespace baikal
