/* Unit tests: lib/Fsm hierarchical state machine engine. */
#include <string.h>

#include "Fsm.h"
#include "unity.h"

/*
 * Test machine:
 *   ROOT
 *   +-- A (initial A1)
 *   |   +-- A1   NEXT -> A2
 *   |   +-- A2   NEXT -> B, SELF -> A2, TO_A -> A, REENTER -> nested dispatch
 *   +-- B        UP -> A
 *   C (top level, outside ROOT)
 *   ROOT handles GLOBAL -> C
 */
enum
{
    EV_NEXT = 1,
    EV_SELF,
    EV_UP,
    EV_GLOBAL,
    EV_TO_A,
    EV_UNKNOWN,
    EV_REENTER
};

static char            g_trace[256];
static Fsm_MachineType g_m;
static Fsm_ResultType  g_nestedResult;

static void Trace(const char *s)
{
    strncat(g_trace, s, sizeof(g_trace) - strlen(g_trace) - 1u);
}

#define ACTIONS(NAME)                                                                                                 \
    static void NAME##_Entry(Fsm_MachineType *m)                                                                      \
    {                                                                                                                 \
        (void)m;                                                                                                      \
        Trace("+" #NAME);                                                                                             \
    }                                                                                                                 \
    static void NAME##_Exit(Fsm_MachineType *m)                                                                       \
    {                                                                                                                 \
        (void)m;                                                                                                      \
        Trace("-" #NAME);                                                                                             \
    }

ACTIONS(ROOT)
ACTIONS(A)
ACTIONS(A1)
ACTIONS(A2)
ACTIONS(B)
ACTIONS(C)

static Fsm_ResultType ROOT_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static Fsm_ResultType A1_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static Fsm_ResultType A2_Handler(Fsm_MachineType *m, const Fsm_EventType *e);
static Fsm_ResultType B_Handler(Fsm_MachineType *m, const Fsm_EventType *e);

static const Fsm_StateType S_ROOT;
static const Fsm_StateType S_A;
static const Fsm_StateType S_A1;
static const Fsm_StateType S_A2;
static const Fsm_StateType S_B;
static const Fsm_StateType S_C;

static const Fsm_StateType S_ROOT = {"ROOT", NULL, &S_A, ROOT_Entry, ROOT_Exit, ROOT_Handler};
static const Fsm_StateType S_A    = {"A", &S_ROOT, &S_A1, A_Entry, A_Exit, NULL};
static const Fsm_StateType S_A1   = {"A1", &S_A, NULL, A1_Entry, A1_Exit, A1_Handler};
static const Fsm_StateType S_A2   = {"A2", &S_A, NULL, A2_Entry, A2_Exit, A2_Handler};
static const Fsm_StateType S_B    = {"B", &S_ROOT, NULL, B_Entry, B_Exit, B_Handler};
static const Fsm_StateType S_C    = {"C", NULL, NULL, C_Entry, C_Exit, NULL};

static Fsm_ResultType ROOT_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    return (e->id == EV_GLOBAL) ? Fsm_Transition(m, &S_C) : FSM_IGNORED;
}

static Fsm_ResultType A1_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    return (e->id == EV_NEXT) ? Fsm_Transition(m, &S_A2) : FSM_IGNORED;
}

static Fsm_ResultType A2_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    switch (e->id)
    {
    case EV_NEXT:
        return Fsm_Transition(m, &S_B);
    case EV_SELF:
        return Fsm_Transition(m, &S_A2);
    case EV_TO_A:
        return Fsm_Transition(m, &S_A);
    case EV_REENTER:
    {
        const Fsm_EventType inner = {EV_NEXT, 0u};
        g_nestedResult            = Fsm_Dispatch(m, &inner);
        return FSM_HANDLED;
    }
    default:
        return FSM_IGNORED;
    }
}

static Fsm_ResultType B_Handler(Fsm_MachineType *m, const Fsm_EventType *e)
{
    return (e->id == EV_UP) ? Fsm_Transition(m, &S_A) : FSM_IGNORED;
}

static Fsm_ResultType Send(Fsm_EventIdType id)
{
    const Fsm_EventType e = {id, 0u};
    g_trace[0]            = '\0';
    return Fsm_Dispatch(&g_m, &e);
}

void setUp(void)
{
    g_trace[0] = '\0';
    TEST_ASSERT_EQUAL(E_OK, Fsm_Init(&g_m, &S_A, NULL, NULL));
}

void tearDown(void)
{
}

static void test_init_enters_ancestors_then_initial_children(void)
{
    TEST_ASSERT_EQUAL_STRING("+ROOT+A+A1", g_trace);
    TEST_ASSERT_EQUAL_PTR(&S_A1, Fsm_GetState(&g_m));
}

static void test_sibling_transition_keeps_parent(void)
{
    TEST_ASSERT_EQUAL(FSM_TRANSITION, Send(EV_NEXT));
    TEST_ASSERT_EQUAL_STRING("-A1+A2", g_trace);
}

static void test_transition_out_of_composite_exits_parent(void)
{
    Send(EV_NEXT);
    Send(EV_NEXT);
    TEST_ASSERT_EQUAL_STRING("-A2-A+B", g_trace);
    TEST_ASSERT_EQUAL_PTR(&S_B, Fsm_GetState(&g_m));
}

static void test_transition_into_composite_drills_to_initial_child(void)
{
    Send(EV_NEXT);
    Send(EV_NEXT);
    Send(EV_UP);
    TEST_ASSERT_EQUAL_STRING("-B+A+A1", g_trace);
}

static void test_self_transition_is_external(void)
{
    Send(EV_NEXT);
    Send(EV_SELF);
    TEST_ASSERT_EQUAL_STRING("-A2+A2", g_trace);
}

static void test_transition_to_own_ancestor_reenters_it(void)
{
    Send(EV_NEXT);
    Send(EV_TO_A);
    TEST_ASSERT_EQUAL_STRING("-A2-A+A+A1", g_trace);
}

static void test_event_bubbles_to_root_and_leaves_hierarchy(void)
{
    TEST_ASSERT_EQUAL(FSM_TRANSITION, Send(EV_GLOBAL));
    TEST_ASSERT_EQUAL_STRING("-A1-A-ROOT+C", g_trace);
    TEST_ASSERT_FALSE(Fsm_IsIn(&g_m, &S_ROOT));
}

static void test_unhandled_event_is_ignored_without_actions(void)
{
    TEST_ASSERT_EQUAL(FSM_IGNORED, Send(EV_UNKNOWN));
    TEST_ASSERT_EQUAL_STRING("", g_trace);
    TEST_ASSERT_EQUAL_PTR(&S_A1, Fsm_GetState(&g_m));
}

static void test_is_in_reports_ancestors(void)
{
    TEST_ASSERT_TRUE(Fsm_IsIn(&g_m, &S_A1));
    TEST_ASSERT_TRUE(Fsm_IsIn(&g_m, &S_A));
    TEST_ASSERT_TRUE(Fsm_IsIn(&g_m, &S_ROOT));
    TEST_ASSERT_FALSE(Fsm_IsIn(&g_m, &S_B));
}

static void test_nested_dispatch_is_rejected(void)
{
    Send(EV_NEXT);
    Send(EV_REENTER);
    TEST_ASSERT_EQUAL(FSM_IGNORED, g_nestedResult);
    TEST_ASSERT_EQUAL_PTR(&S_A2, Fsm_GetState(&g_m));
}

static const Fsm_StateType *g_from;
static const Fsm_StateType *g_to;

static void Recorder(Fsm_MachineType *m, const Fsm_StateType *from, const Fsm_StateType *to)
{
    (void)m;
    g_from = from;
    g_to   = to;
}

static void test_trace_hook(void)
{
    TEST_ASSERT_EQUAL(E_OK, Fsm_Init(&g_m, &S_A, NULL, Recorder));
    TEST_ASSERT_NULL(g_from);
    TEST_ASSERT_EQUAL_PTR(&S_A1, g_to);
    Send(EV_NEXT);
    TEST_ASSERT_EQUAL_PTR(&S_A1, g_from);
    TEST_ASSERT_EQUAL_PTR(&S_A2, g_to);
}

static void test_init_rejects_null(void)
{
    TEST_ASSERT_EQUAL(E_NOT_OK, Fsm_Init(NULL, &S_A, NULL, NULL));
    TEST_ASSERT_EQUAL(E_NOT_OK, Fsm_Init(&g_m, NULL, NULL, NULL));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_enters_ancestors_then_initial_children);
    RUN_TEST(test_sibling_transition_keeps_parent);
    RUN_TEST(test_transition_out_of_composite_exits_parent);
    RUN_TEST(test_transition_into_composite_drills_to_initial_child);
    RUN_TEST(test_self_transition_is_external);
    RUN_TEST(test_transition_to_own_ancestor_reenters_it);
    RUN_TEST(test_event_bubbles_to_root_and_leaves_hierarchy);
    RUN_TEST(test_unhandled_event_is_ignored_without_actions);
    RUN_TEST(test_is_in_reports_ancestors);
    RUN_TEST(test_nested_dispatch_is_rejected);
    RUN_TEST(test_trace_hook);
    RUN_TEST(test_init_rejects_null);
    return UNITY_END();
}
