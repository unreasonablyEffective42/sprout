# Typed Lisp — Frozen Semantics (v0)

## 1) Evaluation: lazy-by-default, single-level

- Call-by-need (lazy with sharing):
  - `let/lets/letr` bindings are thunks, forced on demand, memoized after forcing.

### `force`
- `(force e)` forces `e` to WHNF (implementation boundary), memoizing results.

### `do`
- `(do e1 e2 ... en)` forces each `ei` sequentially (to WHNF), discards intermediate values,
  and returns the value of `en`.

## 1.5) Typed let bindings

- All `let/lets/letr` bindings require an explicit type annotation of the binder:
  - `(x:T rhs)`
- This applies even when `rhs` is a function value (including partial application).
- Polymorphic values must also be annotated with a `forall` type at the binding site.

## 2) Delimited control and effects/handlers

### Backend core
- `reset` delimits control.
- `shift` captures the delimited continuation up to the nearest `reset`.

### One-shot rule
- Captured continuations are one-shot:
  - resuming a captured continuation more than once is a runtime error.

### User-facing effects
- `(perform Op arg1 arg2 ...)` triggers operation `Op`.
- `(handle e
    (Op (x...) k body) ...
    (return (r) retBody))`

Semantics:
- Evaluate/force `e` under an active handler.
- A matching `Op` clause receives:
  - op arguments bound to clause parameters,
  - continuation `k` representing the rest of the computation (one-shot).
- Normal completion routes through the `return` clause.

Unmatched `perform` at top level is a runtime error.

## 3) Errors

### Error ADT value
- `(error tag message [data])` constructs an error value.

### Raise / try-catch (sugar)
- `(raise err)` is sugar for `(perform Raise err)`.
- `(try expr (catch (e) handlerExpr))` is sugar for a `handle` that catches `Raise`.
- Uncaught `Raise` aborts the program with an error report.

## 4) Equality

### `eq?`
- Identity/pointer equality, defined on all runtime values.

### `equal?`
- Structural equality on data (numbers/bools/chars/strings/symbols/lists/vectors/ADTs/quoted data).
- Not defined on functions/closures, continuations, handlers, etc. (type error).
- May force thunks as needed to compare structure.

## 5) System F typing (explicit)

- Type variables must be explicitly bound:
  - in types via `(forall (A ...) type)`
  - in terms via `(tlambda (A ...) expr)`
- Type application is explicit: `(tapply expr T1 T2 ...)`
- No implicit `forall` generalization in v0.

## 6) ADTs, Maybe, and pattern matching

### ADT definition
- `(data TypeName (A...) (Ctor (fieldTy...)) ...)` introduces:
  - a type constructor `TypeName`
  - value constructors `Ctor`

### Optional values
- Optional values are represented by the ADT `Maybe A`:
  - `(Nothing)` and `(Just x)`.

### Match
- `(match scrutinee (Pat body) ... (else body))`
- The scrutinee is forced enough to determine its outer constructor/literal/cons-ness (WHNF).
- If no clause matches and no `else`, runtime match error (v0).

### Pattern forms (v0)
- `_` wildcard (binds nothing)
- `x` binder (IDENT)
- literals (numbers/bools/strings/etc.)
- constructor patterns: `(Just x)`, `(Cons h t)` etc.
- list patterns (Scheme-style):
  - `(x y z)` matches a proper list length 3
  - `(x y . xs)` matches list with >=2 elems; xs is tail (proper or improper)
  - `(x . z)` matches cons cell / improper list

### List/dotted literals
- List literals and dotted pair/improper list literals are only constructed via
  `quote`/`quasiquote` (reader forms `'` and `` ` ``).

## 7) Placeholder partial application

- `_` in expressions is ONLY legal inside application argument lists.
- Applications containing `_` placeholders desugar to a lambda.
- Placeholder order is left-to-right appearance order.

Example:
- `(foo 2 _ 4 _)` desugars to `(lambda (a b) (foo 2 a 4 b))`

Type rule:
- Placeholder parameter types are derived from the callee’s (instantiated) function type during typechecking.
- If the callee is polymorphic (`forall`), it must be instantiated via `tapply` (or otherwise made monomorphic) before hole desugaring.
