#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

class State {
public:  // adhoc
	float m_value; // [s]
	const float m_penalty; // [s]
#define MAX_TRANSITIONS 4
	State *m_transition[MAX_TRANSITIONS];

private:
	float m_last_value;
	bool m_has_last_value;

public:
	State(float value, float penalty = 0) :
		m_value(value), m_penalty(penalty),
		m_last_value(0), m_has_last_value(false) {
		memset(m_transition, NULL, sizeof(m_transition));
	}
	virtual ~State() {};

	bool CanTransit(const State &state) const {
		for (int i = 0; i < MAX_TRANSITIONS && m_transition[i] != NULL; i++) {
			if (m_transition[i] == &state) {
				return true;
			} 
		}
		return false;
	}

	float GetValue(void) const { return m_value - m_penalty - 1/* cost of transition */; }
	
	bool UpdateValue(void) {
		assert(m_transition[0] != NULL);
		if (m_transition[3]) {
			assert(m_transition[0] != NULL);
			assert(m_transition[1] != NULL);
			assert(m_transition[2] != NULL);
			if (m_transition[0]->GetValue() >= m_transition[1]->GetValue()
			    && m_transition[0]->GetValue() >= m_transition[2]->GetValue()
			    && m_transition[0]->GetValue() >= m_transition[3]->GetValue()) {
				m_value = m_transition[0]->GetValue() * 0.7f + m_transition[1]->GetValue() * 0.1f + m_transition[2]->GetValue() * 0.1f + m_transition[3]->GetValue() * 0.1f;
			}
			else if (m_transition[1]->GetValue() >= m_transition[0]->GetValue()
				 && m_transition[1]->GetValue() >= m_transition[2]->GetValue()
				 && m_transition[1]->GetValue() >= m_transition[3]->GetValue()) {
				m_value = m_transition[0]->GetValue() * 0.1f + m_transition[1]->GetValue() * 0.7f + m_transition[2]->GetValue() * 0.1f + m_transition[3]->GetValue() * 0.1f;
			}
			else if (m_transition[2]->GetValue() >= m_transition[0]->GetValue()
				 && m_transition[2]->GetValue() >= m_transition[1]->GetValue()
				 && m_transition[2]->GetValue() >= m_transition[3]->GetValue()) {
				m_value = m_transition[0]->GetValue() * 0.1f + m_transition[1]->GetValue() * 0.1f + m_transition[2]->GetValue() * 0.7f + m_transition[3]->GetValue() * 0.1f;
			}
			else {
				assert(m_transition[3]->GetValue() >= m_transition[0]->GetValue());
				assert(m_transition[3]->GetValue() >= m_transition[1]->GetValue());
				assert(m_transition[3]->GetValue() >= m_transition[2]->GetValue());
				m_value = m_transition[0]->GetValue() * 0.1f + m_transition[1]->GetValue() * 0.1f + m_transition[2]->GetValue() * 0.1f + m_transition[2]->GetValue() * 0.7f;
			}
		} else if (m_transition[2]) {
			assert(m_transition[0] != NULL);
			assert(m_transition[1] != NULL);
			if (m_transition[0]->GetValue() >= m_transition[1]->GetValue()
			    && m_transition[0]->GetValue() >= m_transition[2]->GetValue()) {
				m_value = m_transition[0]->GetValue() * 0.8f + m_transition[1]->GetValue() * 0.1f + m_transition[2]->GetValue() * 0.1f;
			}
			else if (m_transition[1]->GetValue() >= m_transition[0]->GetValue()
				 && m_transition[1]->GetValue() >= m_transition[2]->GetValue()) {
				m_value = m_transition[0]->GetValue() * 0.1f + m_transition[1]->GetValue() * 0.8f + m_transition[2]->GetValue() * 0.1f;
			}
			else {
				assert(m_transition[2]->GetValue() >= m_transition[0]->GetValue());
				assert(m_transition[2]->GetValue() >= m_transition[1]->GetValue());
				m_value = m_transition[0]->GetValue() * 0.1f + m_transition[1]->GetValue() * 0.1f + m_transition[2]->GetValue() * 0.8f;
			}
		} else if (m_transition[1]) {
			assert(m_transition[0] != NULL);
			if (m_transition[0]->GetValue() >= m_transition[1]->GetValue()) {
				m_value = m_transition[0]->GetValue() * 0.9f + m_transition[1]->GetValue() * 0.1f;
			} else {
				assert(m_transition[1]->GetValue() >= m_transition[0]->GetValue());
				m_value = m_transition[0]->GetValue() * 0.1f + m_transition[1]->GetValue() * 0.9f;
			}
		} else if (m_transition[0]) {
			assert(false);
		} else {
			assert(false);
		}

		{ // convergence test
#define MY_EPSILON 0.1f // adhoc
			const bool converged = (m_has_last_value && (fabs(m_last_value - m_value) < MY_EPSILON));
#undef MY_EPSILON
			m_last_value = m_value;
			m_has_last_value = true;
			return converged;
		}
	}
};

static bool link(State &s1, State &s2, bool bi_directional = true) {
	{
		int i;
		for (i = 0; i < MAX_TRANSITIONS; i++) {
			if (s1.m_transition[i] == NULL) {
				s1.m_transition[i] = &s2;
				break;
			}
		}
		assert(i < MAX_TRANSITIONS);
#undef MAX_TRANSITIONS
	}

	if (bi_directional) {
		return link(s2, s1, false);
	} else {
		return true;
	}
}

int main(int argc, char *argv[])
{
	printf("finite MDPs :: `value iteration'\n");

	for (int penalty = 0; penalty <= 500; penalty++) { 
#define PENALTY 10
		State s1(-100.0f), s2(-100.0f, penalty), sg(0); 
#undef PENALTY
		State s4(-100.0f), s5(-100.0f), s6(-100.0f);
		State s7(-100.0f), s8(-100.0f), s9(-100.0f);
		State g2(0);

		link(s1, s2); link(s2, sg);
		link(s1, s4); link(s2, s5); link(sg, s6);
		link(s4, s5); link(s5, s6);
		link(s4, s7); link(s5, s8); link(s6, s9);
		link(s7, s8); link(s8, s9);
		link(s9, g2);

		printf("\n");
		printf("### state diagram ###\n");
		printf("s1 %s s2 %s g1\n", s2.CanTransit(s1) ? "-" : " ", sg.CanTransit(s2) ? "-" : " ");
		assert(s1.CanTransit(s2) == s2.CanTransit(s1));
		assert(s2.CanTransit(sg) == sg.CanTransit(s2));
		printf("%s    %s    %s\n",
		       s4.CanTransit(s1) ? "|" : " ", s5.CanTransit(s2) ? "|" : " ", s6.CanTransit(sg) ? "|" : " ");
		assert(s1.CanTransit(s4) == s4.CanTransit(s1));
		assert(s2.CanTransit(s5) == s5.CanTransit(s2));
		assert(sg.CanTransit(s6) == s6.CanTransit(sg));
		printf("s4 %s s5 %s s6\n", s5.CanTransit(s4) ? "-" : " ", s6.CanTransit(s5) ? "-" : " ");
		assert(s4.CanTransit(s5) == s5.CanTransit(s4));
		assert(s5.CanTransit(s6) == s6.CanTransit(s5));
		printf("%s    %s    %s\n",
		       s7.CanTransit(s4) ? "|" : " ", s8.CanTransit(s5) ? "|" : " ", s9.CanTransit(s6) ? "|" : " ");
		assert(s4.CanTransit(s7) == s7.CanTransit(s4));
		assert(s5.CanTransit(s8) == s8.CanTransit(s5));
		assert(s6.CanTransit(s9) == s9.CanTransit(s6));
		printf("s7 %s s8 %s s9\n", s7.CanTransit(s8) ? "-" : " ", s8.CanTransit(s9) ? "-" : " ");
		assert(s8.CanTransit(s7) == s7.CanTransit(s8));
		assert(s9.CanTransit(s8) == s8.CanTransit(s9));
		printf("          %s\n", s9.CanTransit(g2) ? "|" : " ");
		assert(g2.CanTransit(s9) == s9.CanTransit(g2));
		printf("          g2\n");

		printf("\n");
		printf("### initial value ###\n");
		printf("%6.1f\t%6.1f\t%6.1f\n", s1.m_value, s2.m_value, sg.m_value);
		printf("%6.1f\t%6.1f\t%6.1f\n", s4.m_value, s5.m_value, s6.m_value);
		printf("%6.1f\t%6.1f\t%6.1f\n", s7.m_value, s8.m_value, s9.m_value);
		printf("      \t      \t%6.1f\n", g2.m_value);

		printf("\n");
		printf("### penalty ###\n");
		printf("%6.1f\t%6.1f\t%6.1f\n", s1.m_penalty, s2.m_penalty, sg.m_penalty);
		printf("%6.1f\t%6.1f\t%6.1f\n", s4.m_penalty, s5.m_penalty, s6.m_penalty);
		printf("%6.1f\t%6.1f\t%6.1f\n", s7.m_penalty, s8.m_penalty, s9.m_penalty);
		printf("      \t      \t%6.1f\n", g2.m_penalty);
	
		{
			int count = 0;
			while ((
#if 1 // calculation order
				       s1.UpdateValue() && s2.UpdateValue()
				       && s4.UpdateValue() && s5.UpdateValue() && s6.UpdateValue()
				       && s7.UpdateValue() && s8.UpdateValue() && s9.UpdateValue()
#else
				       s2.UpdateValue() && s6.UpdateValue()
				       && s5.UpdateValue()
				       && s1.UpdateValue() && s9.UpdateValue()
				       && s4.UpdateValue() && s8.UpdateValue() 
				       && s7.UpdateValue()  
#endif
				       ) == false) {
				count++;
			}
			printf("\n");
			printf("### sweep %d ###\n", count);
			printf("%6.1f\t%6.1f\t%6.1f\n", s1.m_value, s2.m_value, sg.m_value);
			printf("%6.1f\t%6.1f\t%6.1f\n", s4.m_value, s5.m_value, s6.m_value);
			printf("%6.1f\t%6.1f\t%6.1f\n", s7.m_value, s8.m_value, s9.m_value);
			printf("      \t      \t%6.1f\n", g2.m_value);
		}
	}
	
   	return 0;
}
