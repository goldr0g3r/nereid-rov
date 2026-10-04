/**
 * @file    Fsm.h
 * @brief   Small hierarchical state machine (HSM) engine.
 *
 * Design (see docs/decisions/ADR-006-state-machine-engine.md):
 *  - States are const descriptors with an optional parent, optional initial child,
 *    entry/exit actions and an event handler.
 *  - The active configuration is always a leaf state.
 *  - Fsm_Dispatch() offers the event to the leaf, then to each ancestor until one
 *    handles it (UML statechart "bubbling").
 *  - A handler requests a transition with `return Fsm_Transition(m, &TARGET);`.
 *    Transitions are external: exit up to the least common ancestor, enter down to
 *    the target, then follow `initial` children down to a leaf.
 *  - Run-to-completion, no dynamic memory, no recursion, bounded nesting depth.
 *
 * The engine knows nothing about queues or time: callers own their event sources.
 */
#ifndef FSM_H
#define FSM_H

#include "Std_Types.h"

/** Maximum nesting depth (root counts as 1). */
#define FSM_MAX_DEPTH 8u

typedef uint16_t Fsm_EventIdType;

typedef struct
{
    Fsm_EventIdType id;
    uint32_t        param; /**< optional payload, meaning defined by the machine */
} Fsm_EventType;

typedef enum
{
    FSM_IGNORED = 0, /**< not handled here, offer to parent */
    FSM_HANDLED,     /**< consumed, no state change */
    FSM_TRANSITION   /**< consumed, transition requested via Fsm_Transition() */
} Fsm_ResultType;

struct Fsm_Machine;

typedef void (*Fsm_ActionFn)(struct Fsm_Machine *m);
typedef Fsm_ResultType (*Fsm_HandlerFn)(struct Fsm_Machine *m, const Fsm_EventType *e);

typedef struct Fsm_State
{
    const char             *name;
    const struct Fsm_State *parent;  /**< NULL for top-level states */
    const struct Fsm_State *initial; /**< default child for composite states, else NULL */
    Fsm_ActionFn            entry;   /**< optional */
    Fsm_ActionFn            exit;    /**< optional */
    Fsm_HandlerFn           handler; /**< optional */
} Fsm_StateType;

typedef void (*Fsm_TraceFn)(struct Fsm_Machine *m, const Fsm_StateType *from, const Fsm_StateType *to);

typedef struct Fsm_Machine
{
    const Fsm_StateType *current; /**< active leaf state */
    const Fsm_StateType *target;  /**< pending transition target (internal) */
    void                *context; /**< user data for the owning component */
    Fsm_TraceFn          trace;   /**< optional transition observer */
    bool                 busy;    /**< guards against re-entrant dispatch */
} Fsm_MachineType;

/**
 * Initialise the machine and enter @p initial (and its ancestors / initial children).
 * @return E_NOT_OK on invalid arguments or nesting deeper than FSM_MAX_DEPTH.
 */
Std_ReturnType Fsm_Init(Fsm_MachineType *m, const Fsm_StateType *initial, void *context, Fsm_TraceFn trace);

/** Dispatch one event (run-to-completion). Returns how the event was consumed. */
Fsm_ResultType Fsm_Dispatch(Fsm_MachineType *m, const Fsm_EventType *e);

/** Request a transition from inside a handler. Always returns FSM_TRANSITION. */
Fsm_ResultType Fsm_Transition(Fsm_MachineType *m, const Fsm_StateType *target);

/** True if @p state is the active leaf or one of its ancestors. */
bool Fsm_IsIn(const Fsm_MachineType *m, const Fsm_StateType *state);

/** Active leaf state. */
const Fsm_StateType *Fsm_GetState(const Fsm_MachineType *m);

#endif /* FSM_H */
