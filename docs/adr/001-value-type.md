# ADR-001: Value Type Representation

## Date
2026-05-09

## Status
Accepted

## Context
The VM needs a single type that can hold all CVM++ values: integers, booleans, and nil. This type is used everywhere - on the stack, in the constant pool, in variables. The choice affects performance, safety, and code clarity.

## Decision
Use std::variant<int64_t, bool, std::monostate> where monostate represents nil. Wrap it in a type alias: using Value = std::variant<int64_t, bool, std::monostate>.

## Consequences
- Easier: type-safe access via std::get and std::holds_alternative, no manual tag management, compiler catches missing cases in std::visit
- Harder: slightly more verbose than a raw tagged union struct
- Impossible: storing a raw pointer in Value without extending the variant (needed later for strings and objects)
