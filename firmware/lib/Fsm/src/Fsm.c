/**
 * @file    Fsm.c
 * @brief   Hierarchical state machine engine. See Fsm.h for semantics.
 */
#include "Fsm.h"

static bool Fsm_IsProperAncestor(const Fsm_StateType *ancestor, const Fsm_StateType *state)
{
    const Fsm_StateType *s = (state != NULL) ? state->parent : NULL;

    while (s != NULL)
    {
        if (s == ancestor)
        {
            return true;
        }
        s = s->parent;
    }
    return false;
}

static void Fsm_Enter(Fsm_MachineType *m, const Fsm_StateType *s)
{
    if (s->entry != NULL)
    {
        s->entry(m);
    }
}

static void Fsm_Exit(Fsm_MachineType *m, const Fsm_StateType *s)
{
    if (s->exit != NULL)
    {
        s->exit(m);
    }
}

/* Enter every state on the path (stop, target] from the outside in, then drill
 * into initial children. Returns the resulting leaf, or NULL if too deep. */
static const Fsm_StateType *Fsm_EnterPath(Fsm_MachineType *m, const Fsm_StateType *stop, const Fsm_StateType *target)
{
    const Fsm_StateType *path[FSM_MAX_DEPTH];
    uint32_t             depth = 0u;
    const Fsm_StateType *s     = target;

    while ((s != NULL) && (s != stop))
    {
        if (depth >= FSM_MAX_DEPTH)
        {
            return NULL;
        }
        path[depth] = s;
        depth++;
        s = s->parent;
    }

    while (depth > 0u)
    {
        depth--;
        Fsm_Enter(m, path[depth]);
    }

    s = target;
    while (s->initial != NULL)
    {
        s = s->initial;
        Fsm_Enter(m, s);
    }
    return s;
}

static void Fsm_Execute(Fsm_MachineType *m)
{
    const Fsm_StateType *from   = m->current;
    const Fsm_StateType *target = m->target;
    const Fsm_StateType *s      = m->current;

    m->target = NULL;

    /* Exit until we reach a proper ancestor of the target (the LCA). For a
     * self-transition this exits and re-enters the target (external semantics). */
    while ((s != NULL) && !Fsm_IsProperAncestor(s, target))
    {
        Fsm_Exit(m, s);
        s = s->parent;
    }

    m->current = Fsm_EnterPath(m, s, target);

    if (m->trace != NULL)
    {
        m->trace(m, from, m->current);
    }
}

Std_ReturnType Fsm_Init(Fsm_MachineType *m, const Fsm_StateType *initial, void *context, Fsm_TraceFn trace)
{
    if ((m == NULL) || (initial == NULL))
    {
        return E_NOT_OK;
    }

    m->current = NULL;
    m->target  = NULL;
    m->context = context;
    m->trace   = trace;
    m->busy    = true;

    m->current = Fsm_EnterPath(m, NULL, initial);
    m->busy    = false;

    if (m->current == NULL)
    {
        return E_NOT_OK;
    }
    if (m->trace != NULL)
    {
        m->trace(m, NULL, m->current);
    }
    return E_OK;
}

Fsm_ResultType Fsm_Dispatch(Fsm_MachineType *m, const Fsm_EventType *e)
{
    const Fsm_StateType *s;
    Fsm_ResultType       result = FSM_IGNORED;

    if ((m == NULL) || (e == NULL) || (m->current == NULL) || m->busy)
    {
        return FSM_IGNORED;
    }

    m->busy = true;
    s       = m->current;

    while ((s != NULL) && (result == FSM_IGNORED))
    {
        if (s->handler != NULL)
        {
            result = s->handler(m, e);
        }
        s = s->parent;
    }

    if ((result == FSM_TRANSITION) && (m->target != NULL))
    {
        Fsm_Execute(m);
    }
    m->target = NULL;
    m->busy   = false;
    return result;
}

Fsm_ResultType Fsm_Transition(Fsm_MachineType *m, const Fsm_StateType *target)
{
    m->target = target;
    return FSM_TRANSITION;
}

bool Fsm_IsIn(const Fsm_MachineType *m, const Fsm_StateType *state)
{
    const Fsm_StateType *s = m->current;

    while (s != NULL)
    {
        if (s == state)
        {
            return true;
        }
        s = s->parent;
    }
    return false;
}

const Fsm_StateType *Fsm_GetState(const Fsm_MachineType *m)
{
    return m->current;
}
