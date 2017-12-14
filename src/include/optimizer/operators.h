
//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// operators.h
//
// Identification: src/include/optimizer/operators.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "optimizer/column.h"
#include "optimizer/group.h"
#include "optimizer/operator_node.h"
#include "type/types.h"

#include <vector>

namespace peloton {

namespace expression {
class AbstractExpression;
}

namespace parser {
class UpdateClause;
}

namespace storage {
class DataTable;
}

namespace index {
class Index;
}

namespace optimizer {
class PropertySort;
//===--------------------------------------------------------------------===//
// Leaf
//===--------------------------------------------------------------------===//
class LeafOperator : OperatorNode<LeafOperator> {
 public:
  static Operator make(GroupID group);

  GroupID origin_group;
};

//===--------------------------------------------------------------------===//
// Get
//===--------------------------------------------------------------------===//
class LogicalGet : public OperatorNode<LogicalGet> {
 public:
  static Operator make(oid_t get_id = 0,
                       std::vector<AnnotatedExpression> predicates = {},
                       storage::DataTable *table = nullptr,
                       std::string alias = "",
                       bool update = false);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  // identifier for all get operators
  oid_t get_id;
  std::vector<AnnotatedExpression> predicates;
  storage::DataTable *table;
  std::string table_alias;
  bool is_for_update;
};

//===--------------------------------------------------------------------===//
// Query derived get
//===--------------------------------------------------------------------===//
class LogicalQueryDerivedGet : public OperatorNode<LogicalQueryDerivedGet> {
 public:
  static Operator make(oid_t get_id, std::string& alias,
                       std::unordered_map<std::string,
                                          std::shared_ptr<expression::AbstractExpression>> alias_to_expr_map);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  // identifier for all get operators
  oid_t get_id;
  std::string table_alias;
  std::unordered_map<std::string, std::shared_ptr<expression::AbstractExpression>>
      alias_to_expr_map;
};

//===--------------------------------------------------------------------===//
// Select
//===--------------------------------------------------------------------===//
class LogicalFilter : public OperatorNode<LogicalFilter> {
 public:
  static Operator make(std::vector<AnnotatedExpression>& filter);
  std::vector<AnnotatedExpression> predicates;

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;
};

//===--------------------------------------------------------------------===//
// Project
//===--------------------------------------------------------------------===//
class LogicalProjection: public OperatorNode<LogicalProjection> {
 public:
  static Operator make(std::vector<std::shared_ptr<expression::AbstractExpression>>& elements);
  std::vector<std::shared_ptr<expression::AbstractExpression>> expressions;
};

//===--------------------------------------------------------------------===//
// DependentJoin
//===--------------------------------------------------------------------===//
class LogicalDependentJoin: public OperatorNode<LogicalDependentJoin> {
 public:
  static Operator make();

  static Operator make(std::vector<AnnotatedExpression>& conditions);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  std::vector<AnnotatedExpression> join_predicates;
};

//===--------------------------------------------------------------------===//
// MarkJoin
//===--------------------------------------------------------------------===//
class LogicalMarkJoin: public OperatorNode<LogicalMarkJoin> {
 public:
  static Operator make();

  static Operator make(std::vector<AnnotatedExpression>& conditions);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  std::vector<AnnotatedExpression> join_predicates;
};


//===--------------------------------------------------------------------===//
// InnerJoin
//===--------------------------------------------------------------------===//
class LogicalInnerJoin : public OperatorNode<LogicalInnerJoin> {
 public:
  static Operator make();

  static Operator make(std::vector<AnnotatedExpression>& conditions);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  std::vector<AnnotatedExpression> join_predicates;
};

//===--------------------------------------------------------------------===//
// LeftJoin
//===--------------------------------------------------------------------===//
class LogicalLeftJoin : public OperatorNode<LogicalLeftJoin> {
 public:
  static Operator make(expression::AbstractExpression *condition = nullptr);

  std::shared_ptr<expression::AbstractExpression> join_predicate;
};

//===--------------------------------------------------------------------===//
// RightJoin
//===--------------------------------------------------------------------===//
class LogicalRightJoin : public OperatorNode<LogicalRightJoin> {
 public:
  static Operator make(expression::AbstractExpression *condition = nullptr);

  std::shared_ptr<expression::AbstractExpression> join_predicate;
};

//===--------------------------------------------------------------------===//
// OuterJoin
//===--------------------------------------------------------------------===//
class LogicalOuterJoin : public OperatorNode<LogicalOuterJoin> {
 public:
  static Operator make(expression::AbstractExpression *condition = nullptr);

  std::shared_ptr<expression::AbstractExpression> join_predicate;
};

//===--------------------------------------------------------------------===//
// SemiJoin
//===--------------------------------------------------------------------===//
class LogicalSemiJoin : public OperatorNode<LogicalSemiJoin> {
 public:
  static Operator make(expression::AbstractExpression *condition = nullptr);

  std::shared_ptr<expression::AbstractExpression> join_predicate;
};

//===--------------------------------------------------------------------===//
// Aggregate
//===--------------------------------------------------------------------===//
class LogicalAggregate : public OperatorNode<LogicalAggregate> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// GroupBy
//===--------------------------------------------------------------------===//
class LogicalGroupBy : public OperatorNode<LogicalGroupBy> {
 public:
  static Operator make();

  static Operator make(
      std::vector<std::shared_ptr<expression::AbstractExpression>>& columns,
      std::shared_ptr<expression::AbstractExpression>& having);

  std::vector<std::shared_ptr<expression::AbstractExpression>> columns;
  std::shared_ptr<expression::AbstractExpression> having;
};

//===--------------------------------------------------------------------===//
// Insert
//===--------------------------------------------------------------------===//
class LogicalInsert : public OperatorNode<LogicalInsert> {
 public:
  static Operator make(
      storage::DataTable *target_table, const std::vector<std::string> *columns,
      const std::vector<std::vector<std::unique_ptr<expression::AbstractExpression>>> *
          values);

  storage::DataTable *target_table;
  const std::vector<std::string> *columns;
  const std::vector<std::vector<std::unique_ptr<expression::AbstractExpression>>> *values;
};

class LogicalInsertSelect : public OperatorNode<LogicalInsertSelect> {
 public:
  static Operator make(storage::DataTable *target_table);

  storage::DataTable *target_table;
};

//===--------------------------------------------------------------------===//
// Delete
//===--------------------------------------------------------------------===//
class LogicalDelete : public OperatorNode<LogicalDelete> {
 public:
  static Operator make(storage::DataTable *target_table);

  storage::DataTable *target_table;
};

//===--------------------------------------------------------------------===//
// Update
//===--------------------------------------------------------------------===//
class LogicalUpdate : public OperatorNode<LogicalUpdate> {
 public:
  static Operator make(storage::DataTable *target_table,
                       const std::vector<std::unique_ptr<parser::UpdateClause>>* updates);

  storage::DataTable *target_table;
  const std::vector<std::unique_ptr<parser::UpdateClause>>* updates;
};

//===--------------------------------------------------------------------===//
// DummyScan
//===--------------------------------------------------------------------===//
class DummyScan : public OperatorNode<DummyScan> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// SeqScan
//===--------------------------------------------------------------------===//
class PhysicalSeqScan : public OperatorNode<PhysicalSeqScan> {
 public:
  static Operator make(oid_t get_id, storage::DataTable *table, std::string alias,
                       std::vector<AnnotatedExpression> predicates,
                       bool update);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  // identifier for all get operators
  oid_t get_id;
  std::vector<AnnotatedExpression> predicates;
  std::string table_alias;
  bool is_for_update;
  storage::DataTable *table_;
};

//===--------------------------------------------------------------------===//
// IndexScan
//===--------------------------------------------------------------------===//
class PhysicalIndexScan : public OperatorNode<PhysicalIndexScan> {
 public:
  static Operator make(oid_t get_id, storage::DataTable *table, std::string alias,
                       std::vector<AnnotatedExpression> predicates,
                       bool update, oid_t index_id, std::shared_ptr<index::Index> index_obj,
                       std::vector<oid_t> key_column_id_list,
                       std::vector<ExpressionType> expr_type_list,
                       std::vector<type::Value> value_list);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  // identifier for all get operators
  oid_t get_id;
  std::vector<AnnotatedExpression> predicates;
  std::string table_alias;
  bool is_for_update;
  storage::DataTable *table_;

  // Index info.
  // Match planner::IndexScanPlan::IndexScanDesc index_scan_desc(
  //      index, key_column_ids, expr_types, values, runtime_keys)
  oid_t index_id = -1;
  std::shared_ptr<index::Index> index_obj;
  std::vector<oid_t> key_column_id_list;
  std::vector<ExpressionType> expr_type_list;
  std::vector<type::Value> value_list;
};

//===--------------------------------------------------------------------===//
// Query derived get
//===--------------------------------------------------------------------===//
class QueryDerivedScan : public OperatorNode<QueryDerivedScan> {
 public:
  static Operator make(oid_t get_id, std::string alias,
                       std::unordered_map<std::string,
                                          std::shared_ptr<expression::AbstractExpression>> alias_to_expr_map);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  // identifier for all get operators
  oid_t get_id;
  std::string table_alias;
  std::unordered_map<std::string, std::shared_ptr<expression::AbstractExpression>>
      alias_to_expr_map;
};

//===--------------------------------------------------------------------===//
// PhysicalProject
//===--------------------------------------------------------------------===//
class PhysicalProject : public OperatorNode<PhysicalProject> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// PhysicalOrderBy
//===--------------------------------------------------------------------===//
class PhysicalOrderBy : public OperatorNode<PhysicalOrderBy> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// PhysicalLimit
//===--------------------------------------------------------------------===//
class PhysicalLimit : public OperatorNode<PhysicalLimit> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// Filter
//===--------------------------------------------------------------------===//
class PhysicalFilter : public OperatorNode<PhysicalFilter> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// InnerNLJoin
//===--------------------------------------------------------------------===//
class PhysicalInnerNLJoin : public OperatorNode<PhysicalInnerNLJoin> {
 public:
  static Operator make(std::vector<AnnotatedExpression> conditions);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  std::vector<AnnotatedExpression> join_predicates;
};

//===--------------------------------------------------------------------===//
// LeftNLJoin
//===--------------------------------------------------------------------===//
class PhysicalLeftNLJoin : public OperatorNode<PhysicalLeftNLJoin> {
 public:
  std::shared_ptr<expression::AbstractExpression> join_predicate;
  static Operator make(std::shared_ptr<expression::AbstractExpression> join_predicate);
};

//===--------------------------------------------------------------------===//
// RightNLJoin
//===--------------------------------------------------------------------===//
class PhysicalRightNLJoin : public OperatorNode<PhysicalRightNLJoin> {
 public:
  std::shared_ptr<expression::AbstractExpression> join_predicate;
  static Operator make(std::shared_ptr<expression::AbstractExpression> join_predicate);
};

//===--------------------------------------------------------------------===//
// OuterNLJoin
//===--------------------------------------------------------------------===//
class PhysicalOuterNLJoin : public OperatorNode<PhysicalOuterNLJoin> {
 public:
  std::shared_ptr<expression::AbstractExpression> join_predicate;
  static Operator make(std::shared_ptr<expression::AbstractExpression> join_predicate);
};

//===--------------------------------------------------------------------===//
// InnerHashJoin
//===--------------------------------------------------------------------===//
class PhysicalInnerHashJoin : public OperatorNode<PhysicalInnerHashJoin> {
 public:
  static Operator make(std::vector<AnnotatedExpression> conditions);

  bool operator==(const BaseOperatorNode &r) override;

  hash_t Hash() const override;

  std::vector<AnnotatedExpression> join_predicates;
};

//===--------------------------------------------------------------------===//
// LeftHashJoin
//===--------------------------------------------------------------------===//
class PhysicalLeftHashJoin : public OperatorNode<PhysicalLeftHashJoin> {
 public:
  std::shared_ptr<expression::AbstractExpression> join_predicate;
  static Operator make(std::shared_ptr<expression::AbstractExpression> join_predicate);
};

//===--------------------------------------------------------------------===//
// RightHashJoin
//===--------------------------------------------------------------------===//
class PhysicalRightHashJoin : public OperatorNode<PhysicalRightHashJoin> {
 public:
  std::shared_ptr<expression::AbstractExpression> join_predicate;
  static Operator make(std::shared_ptr<expression::AbstractExpression> join_predicate);
};

//===--------------------------------------------------------------------===//
// OuterHashJoin
//===--------------------------------------------------------------------===//
class PhysicalOuterHashJoin : public OperatorNode<PhysicalOuterHashJoin> {
 public:
  std::shared_ptr<expression::AbstractExpression> join_predicate;
  static Operator make(std::shared_ptr<expression::AbstractExpression> join_predicate);
};

//===--------------------------------------------------------------------===//
// PhysicalInsert
//===--------------------------------------------------------------------===//
class PhysicalInsert : public OperatorNode<PhysicalInsert> {
 public:
  static Operator make(
      storage::DataTable *target_table, const std::vector<std::string> *columns,
      const std::vector<std::vector<std::unique_ptr<expression::AbstractExpression>>> *
          values);

  storage::DataTable *target_table;
  const std::vector<std::string> *columns;
  const std::vector<std::vector<std::unique_ptr<expression::AbstractExpression>>> *values;
};

class PhysicalInsertSelect : public OperatorNode<PhysicalInsertSelect> {
 public:
  static Operator make(storage::DataTable *target_table);

  storage::DataTable *target_table;
};

//===--------------------------------------------------------------------===//
// PhysicalDelete
//===--------------------------------------------------------------------===//
class PhysicalDelete : public OperatorNode<PhysicalDelete> {
 public:
  static Operator make(storage::DataTable *target_table);
  storage::DataTable *target_table;
};

//===--------------------------------------------------------------------===//
// PhysicalUpdate
//===--------------------------------------------------------------------===//
class PhysicalUpdate : public OperatorNode<PhysicalUpdate> {
 public:
  static Operator make(storage::DataTable *target_table,
  const std::vector<std::unique_ptr<parser::UpdateClause>>* updates);

  storage::DataTable *target_table;
  const std::vector<std::unique_ptr<parser::UpdateClause>>* updates;
};

//===--------------------------------------------------------------------===//
// PhysicalHashGroupBy
//===--------------------------------------------------------------------===//
class PhysicalHashGroupBy : public OperatorNode<PhysicalHashGroupBy> {
 public:
  static Operator make(
      std::vector<std::shared_ptr<expression::AbstractExpression>> columns,
      expression::AbstractExpression *having);

  bool operator==(const BaseOperatorNode &r) override;
  hash_t Hash() const override;

  std::vector<std::shared_ptr<expression::AbstractExpression>> columns;
  expression::AbstractExpression *having;
};

//===--------------------------------------------------------------------===//
// PhysicalSortGroupBy
//===--------------------------------------------------------------------===//
class PhysicalSortGroupBy : public OperatorNode<PhysicalSortGroupBy> {
 public:
  static Operator make(
      std::vector<std::shared_ptr<expression::AbstractExpression>> columns,
      expression::AbstractExpression *having);

  bool operator==(const BaseOperatorNode &r) override;
  hash_t Hash() const override;

  std::vector<std::shared_ptr<expression::AbstractExpression>> columns;
  expression::AbstractExpression *having;
};

//===--------------------------------------------------------------------===//
// PhysicalAggregate
//===--------------------------------------------------------------------===//
class PhysicalAggregate : public OperatorNode<PhysicalAggregate> {
 public:
  static Operator make();
};

class PhysicalDistinct : public OperatorNode<PhysicalDistinct> {
 public:
  static Operator make();
};

} // namespace optimizer
} // namespace peloton
