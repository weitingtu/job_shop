#include "JobShopNode.h"
#include "type.h"
#include <limits.h>
#include <algorithm>

JobShopNode::Job::Job(size_t i, int rr, int pp, int qq) : 
	idx(i), 
	r(rr), 
	p(pp), 
	q(qq), 
	l(r), 
	u(M) 
{
}

void JobShopNode::_initialize_C(JobShopNode& node)
{
	for (size_t i = 1; i < node._jobs.size() - 1; ++i)
	{
		const Job& job = node._jobs.at(i);
		node._C.insert(job.idx);
	}
}

JobShopNode JobShopNode::create_initial_node()
{
	JobShopNode node;
	node._root = true;

	node._jobs.push_back(Job(0,  0,  0,  0)); // S
	node._jobs.push_back(Job(1,  0,  7, 16)); 
	node._jobs.push_back(Job(2,  6, 10,  8));
	node._jobs.push_back(Job(3, 10,  4,  0));
	node._jobs.push_back(Job(4,  0,  0,  0)); // T

	node._arcs.insert(std::make_pair(ArcFromTo(0, 1), Arc(0, M)));
	node._arcs.insert(std::make_pair(ArcFromTo(0, 2), Arc(6, M)));
	node._arcs.insert(std::make_pair(ArcFromTo(0, 3), Arc(10, M)));
	//node._arcs.insert(std::make_pair(ArcFromTo(1, 2), Arc(2, 16)));
	//node._arcs.insert(std::make_pair(ArcFromTo(1, 3), Arc(1, 38)));
	//node._arcs.insert(std::make_pair(ArcFromTo(2, 1), Arc(3, 18)));
	//node._arcs.insert(std::make_pair(ArcFromTo(2, 3), Arc(3, 28)));
	//node._arcs.insert(std::make_pair(ArcFromTo(3, 1), Arc(3, 28)));
	//node._arcs.insert(std::make_pair(ArcFromTo(3, 2), Arc(4, 20)));

	node._arcs.insert(std::make_pair(ArcFromTo(1, 2), Arc(0, 16)));
	node._arcs.insert(std::make_pair(ArcFromTo(1, 3), Arc(0, 38)));
	node._arcs.insert(std::make_pair(ArcFromTo(2, 1), Arc(0, 18)));
	node._arcs.insert(std::make_pair(ArcFromTo(2, 3), Arc(0, 28)));
	node._arcs.insert(std::make_pair(ArcFromTo(3, 1), Arc(0, 28)));
	node._arcs.insert(std::make_pair(ArcFromTo(3, 2), Arc(0, 20)));

	_initialize_C(node);

	return node;
}

JobShopNode::JobShopNode():
	_LB(M),
	_root(true),
	_IS(),
	_OS(),
	_C(),
	_E(),
	_S(),
	_jobs(),
	_arcs()
{
}

void JobShopNode::erase_E(size_t e)
{
	if (_E.find(e) == _E.end())
	{
		printf("error, cannot find %zu in E\n", e);
		return;
	}

	_E.erase(e);
}

void JobShopNode::erase_S(size_t s)
{
	if (_S.find(s) == _S.end())
	{
		printf("error, cannot find %zu in S\n", s);
		return;
	}

	_S.erase(s);
}

void JobShopNode::add_input(size_t e)
{
	if (_C.find(e) == _C.end())
	{
		printf("error, cannot find %zu in C\n", e);
		return;
	}
	_IS.push_back(e);
	_C.erase(e);
}

void JobShopNode::add_output(size_t s)
{
	if (_C.find(s) == _C.end())
	{
		printf("error, cannot find %zu in C\n", s);
		return;
	}
	_OS.insert(_OS.begin(), s);
	_C.erase(s);
}

void JobShopNode::eq4_and_5(size_t e)
{
	Job& job_e = _jobs.at(e);
	if (is_root())
	{
		job_e.l = job_e.r;
		job_e.u = job_e.r;
	}
	for (size_t k : _C)
	{
		Job& job_k = _jobs.at(k);
		// eq 4
		const Arc& arc = get_arc(e, k);
		int r = std::max(job_k.l, job_e.l + job_e.p + arc.W1);
		job_k.r = r;
		job_k.l = r;

		// eq 5
		job_k.u = std::min(job_k.u, job_e.u + job_e.p + arc.W2);
	}
}

bool JobShopNode::eq6_and_7(size_t s)
{
	Job& job_s = _jobs.at(s);

	int l = job_s.r;
	int u = job_s.u;
	for (size_t i : _C)
	{
		if (i == s)
		{
			continue;
		}
		Job& job_i = _jobs.at(i);
		// eq6 
		const Arc& arc = get_arc(i, s);
		l = std::max(l, job_s.l + job_s.p + arc.W1);

		// eq7
		u = std::min(job_i.u, job_s.u + job_s.p + arc.W2);
	}
	job_s.r = l;
	job_s.l = l;
	job_s.u = u;

	return u <= l;
}

void JobShopNode::_eq8_and_9(size_t j, const std::set<size_t>& I)
{
		int l = 0;
		int u = M;
		for (size_t i = 0; i < _jobs.size(); ++i)
		{
			if (i == j)
			{
				continue;
			}
			if (I.find(i) != I.end())
			{
				continue;
			}
			const Job& job = _jobs.at(i);
			const Arc& arc = get_arc(i, j);
			l = std::max(l, job.l + job.p + arc.W1); // eq8
			u = std::min(u, job.u + job.p + arc.W2); // eq9
		}

		Job& job_j = _jobs.at(j);
		int ll = std::max(job_j.l, l);
		job_j.r = ll; // eq8
		job_j.l = ll;

		job_j.u = std::min(job_j.u, u); // eq9
}

void JobShopNode::eq8_and_9()
{
	std::set<size_t> I(_IS.begin(), _IS.end());
	for (size_t j : _OS)
	{
		_eq8_and_9(j, I);
	}
}

void JobShopNode::eq8_and_9(size_t s)
{
	std::set<size_t> I(_IS.begin(), _IS.end());
	for (size_t j : _OS)
	{
		if(j == s)
		{
			continue;
		}
		_eq8_and_9(j, I);
	}
}

void JobShopNode::eq10_and_11(size_t e)
{
	if (is_root())
	{
		printf("error, %s is root\n", __func__);
		return;
	}

	Job& job_e = _jobs.at(e);

	int ll = 0;
	if (!_OS.empty())
	{
		const Job& job_l = _jobs.at(_OS.back());

		ll = job_l.l + job_l.p + job_l.q;
	}
	{
		// eq10
		std::set<size_t> I(_IS.begin(), _IS.end());
		int p = 0;
		int q = M;
		for (size_t i = 0; i < _jobs.size(); ++i)
		{
			if (I.find(i) != I.end())
			{
				continue;
			}
			const Job& job = _jobs.at(i);
			p += job.p;
			q = std::min(q, job.q);
		}

		int qq = std::max(job_e.q, ll - job_e.l - job_e.p);
		job_e.q = std::max(qq, p + q);
	}

	{
		// eq11
		int p = 0;
		int q = M;
		for (size_t i : _OS)
		{
			const Job& job = _jobs.at(i);
			p += job.p;
			q = std::min(q, job.q);
		}
		for (size_t k : _C)
		{
			Job& job_k = _jobs.at(k);
			int qq = std::max(job_k.q, ll - job_k.l - job_k.p);
			job_k.q = std::max(qq, p + q);
		}
	}
}

void JobShopNode::eq11()
{
	if (is_root())
	{
		printf("error, %s is root\n", __func__);
		return;
	}
	int ll = 0;
	if (!_OS.empty())
	{
		const Job& job_l = _jobs.at(_OS.back());

		ll = job_l.l + job_l.p + job_l.q;
	}
	// eq11
	int p = 0;
	int q = M;
	for (size_t i : _OS)
	{
		const Job& job = _jobs.at(i);
		p += job.p;
		q = std::min(q, job.q);
	}
	for (size_t k : _C)
	{
		Job& job_k = _jobs.at(k);
		int qq = std::max(job_k.q, ll - job_k.l - job_k.p);
		job_k.q = std::max(qq, p + q);
	}
}

void JobShopNode::update_root_tail(size_t e)
{
	if (!is_root())
	{
		printf("error, %s is not root\n", __func__);
		return;
	}

	Job& job_e = _jobs.at(e);

	std::set<size_t> I(_IS.begin(), _IS.end());
	int p = 0;
	int q = M;
	for (size_t i = 0; i < _jobs.size(); ++i)
	{
		if (I.find(i) != I.end())
		{
			continue;
		}
		const Job& job = _jobs.at(i);
		p += job.p;
		q = std::min(q, job.q);
	}

	job_e.q = std::max(job_e.q, p + q);
}

int JobShopNode::get_max_l_p_q() const
{
	int UB2 = 0;
	for (size_t i = 0; i < _jobs.size(); ++i)
	{
		if (_C.find(i) != _C.end())
		{
			continue;
		}
		const Job& job = _jobs.at(i);
		UB2 = std::max(UB2, job.l + job.p + job.q);
	}

	return UB2;
}

void JobShopNode::print() const
{
	for (size_t i : _IS)
	{
		printf("%zu ", i);
	}
	for (size_t i : _OS)
	{
		printf("%zu ", i);
	}
	printf("\n");
}

void JobShopNode::calculate_LB()
{
	_LB = _proposition_1();
}

void JobShopNode::get_E(int UB)
{
	_E.clear();
	// proposition 4, 5, 8 and 9
	for (size_t r : _C)
	{
		if (!is_proposition_7(r, UB))
		{
			continue;
        }
		if (is_proposition_4_output(r))
		{
			continue;
		}
		_E.insert(r);
	}
}

void JobShopNode::get_S(int UB)
{
	_S.clear();
	// proposition 4, 5, 8 and 9
	for (size_t k : _C)
	{
		if (!is_proposition_7(k, UB))
		{
			continue;
        }
		if (is_proposition_4_input(k))
		{
			continue;
		}
		_S.insert(k);
	}
}

bool JobShopNode::is_proposition_3(size_t k)
{
	int sum_p = 0;
	for (size_t j : _C)
	{
		if (j == k)
		{
			continue;
		}
		const Job& job = _jobs.at(j);
		sum_p += job.p;
	}
	const Job& job_k = _jobs.at(k);
	for (size_t i : _C)
	{
		if (i == k)
		{
			continue;
		}
		const Job& job = _jobs.at(i);
		if (job.l + sum_p + get_LB_ATSP(_C) > job_k.u)
		{
			return false;
		}
	}

	return true;
}

bool JobShopNode::is_proposition_11(size_t i, size_t j, int UB)
{
	if (_C.size() != 2)
	{
		printf("error, C size %zu is not 2. Failed to evaluate propasition 11\n", _C.size());
		return false;
	}

	Job& job_i = _jobs.at(i);
	Job& job_j = _jobs.at(j);
	const Arc& arci_j = get_arc(i, j);
	const Arc& arcj_i = get_arc(j, i);

	if ((job_j.r + job_j.p + arcj_i.W1 + job_i.r + job_i.q) > UB)
	{
		return false;
	}

	_C.clear();
	_IS.push_back(i);
	_OS.push_back(j);
	job_j.r = std::max(job_j.r, job_i.l + job_i.p + arci_j.W1);
	job_j.l = std::max(job_j.r, job_i.l + job_i.p + arci_j.W1);
	job_j.u = std::max(job_j.u, job_i.u + job_i.p + arci_j.W1);

	job_i.q = std::max(job_i.q, job_j.q + job_j.p + arci_j.W1);

	return true;
}

// algorithm 1
bool JobShopNode::is_feasible(int& C_max) const
{
	std::vector<size_t> seq = _IS;
	seq.insert(seq.end(), _OS.begin(), _OS.end());

	return is_feasible(seq, C_max);
}

bool JobShopNode::is_feasible(const std::vector<size_t>& seq, int& C_max) const
{
	if (seq.empty())
	{
		return false;
	}

	std::vector<int> ll;
	std::vector<int> uu;
    {
	    const Job& job = _jobs.at(seq.front());
	    int l = job.r;
	    int u = job.r;
		C_max = l + job.p + job.q;
		ll.push_back(l);
		uu.push_back(u);
	}
	for (size_t j = 0; j < seq.size(); ++j)
	{
		const Job& job_j = _jobs.at(seq.at(j));
		int l = 0;
		int u = M;
		for (size_t i = 0; i < j; ++i)
		{
		    const Job& job_i = _jobs.at(seq.at(i));
			if (!has_arc(job_i.idx, job_j.idx))
			{
				printf("error, there is no arc between (%zu, %zu)\n", i, j);
				return false;
			}
			const Arc& arc = get_arc(job_i.idx, job_j.idx);
			l = std::max(l, ll[i] + job_i.p + arc.W1);
			u = std::min(u, uu[i] + job_i.p + arc.W2);
		}
		if (l > u)
		{
			return false;
		}
		ll.push_back(l);
		uu.push_back(u);
		C_max = std::max(C_max, l + job_j.p + job_j.q);
		// update W*?
	}
	return true;
}

int JobShopNode::get_H(const std::set<size_t>& C) const
{
	int min_r = M;
	int min_q = M;
	int sum_p = 0;
	for (size_t i : C)
	{
		const Job& job = _jobs.at(i);
		min_r = std::min(min_r, job.r);
		min_q = std::min(min_q, job.q);
		sum_p += job.p;
	}

	return min_r + sum_p + min_q;
}

int JobShopNode::get_LB(const std::set<size_t>& C) const
{
	return get_H(C) + get_LB_ATSP(C);
}

int JobShopNode::get_LB_ATSP(const std::set<size_t>& C) const
{
	(void)C;
	return 0;
}

int JobShopNode::_proposition_1() const
{
	return get_LB(_C);
}

bool JobShopNode::is_proposition_4_input(size_t k) const
{
	const Job& job_k = _jobs.at(k);
	for (size_t r : _C)
	{
	    const Job& job_r = _jobs.at(r);
		// skip initial state
		if (job_r.r == M)
		{
			continue;
		}
		if (!has_arc(k, r))
		{
			printf("error, (%zu, %zu) doesn't have arc\n", k, r);
			continue;
		}

		const Arc& arc = get_arc(k, r);
		if (job_k.l + job_k.p + arc.W1 > job_r.r)
		{
			return true;
		}
		if (job_k.l + job_k.p + arc.W2 < job_r.r)
		{
			return true;
		}
	}

	return false;
}

bool JobShopNode::is_proposition_4_output(size_t r) const
{
	const Job& job_r = _jobs.at(r);
	for (size_t k : _C)
	{
	    const Job& job_k = _jobs.at(k);
		// skip initial state
		if (job_k.r == M)
		{
			continue;
		}
		if (!has_arc(k, r))
		{
			printf("error, (%zu, %zu) doesn't have arc\n", k, r);
			continue;
		}

		const Arc& arc = get_arc(k, r);
		if (job_k.l + job_k.p + arc.W1 > job_r.r)
		{
			return true;
		}
		if (job_k.l + job_k.p + arc.W2 < job_r.r)
		{
			return true;
		}
	}

	return false;
}

bool JobShopNode::is_proposition_5_a(size_t k, int UB) const
{
	int r = _jobs.at(k).r;
	int q = M;
	int sum_p = 0;
	for (size_t i : _C)
	{
		if (k == i)
		{
			continue;
		}
		const Job& job = _jobs.at(i);
		//r = std::min(r, job.r);
		q = std::min(q, job.q);
		sum_p += job.p;
	}

	return r + sum_p + q + get_LB_ATSP(_C) > UB;
}

bool JobShopNode::is_proposition_5_b(size_t idx_r, int UB) const
{
	int r = M;
	int q = _jobs.at(idx_r).q;
	int sum_p = 0;
	for (size_t i : _C)
	{
		if (idx_r == i)
		{
			continue;
		}
		const Job& job = _jobs.at(i);
		r = std::min(r, job.r);
		//q = std::min(q, job.q);
		sum_p += job.p;
	}

	return r + sum_p + q + get_LB_ATSP(_C) > UB;
}

bool JobShopNode::is_proposition_7(size_t k, int UB) const
{
	std::set<size_t> C = _C;;
	C.erase(k);
	int H = get_H(C);

	return H + _jobs.at(k).p > UB;
}

void JobShopNode::proposition_11(int UB)
{
	if (_C.size() != 2)
	{
		printf("error, C size %zu is not 2. Failed to evaluate propasition 11\n", _C.size());
		return;
	}

	std::vector<size_t> C(_C.begin(), _C.end());
	size_t i = C.front();
	size_t j = C.back();

	Job& job_i = _jobs.at(i);
	Job& job_j = _jobs.at(j);
	const Arc& arci_j = get_arc(i, j);
	const Arc& arcj_i = get_arc(j, i);

	if ((job_j.r + job_j.p + arcj_i.W1 + job_i.r + job_i.q) > UB)
	{
		_IS.push_back(i);
		_IS.push_back(j);
		job_j.r = std::max(job_j.r, job_i.l + job_i.p + arci_j.W1);
		job_j.l = std::max(job_j.r, job_i.l + job_i.p + arci_j.W1);
		job_j.u = std::max(job_j.u, job_i.u + job_i.p + arci_j.W1);

		job_i.q = std::max(job_i.q, job_j.q + job_j.p + arci_j.W1);
	}
	else
	{
		_IS.push_back(j);
		_IS.push_back(i);
		job_i.r = std::max(job_i.r, job_j.l + job_j.p + arcj_i.W1);
		job_i.l = std::max(job_i.r, job_j.l + job_j.p + arcj_i.W1);
		job_i.u = std::max(job_i.u, job_j.u + job_j.p + arcj_i.W1);

		job_j.q = std::max(job_j.q, job_i.q + job_i.p + arcj_i.W1);
	}
}

bool JobShopNode::has_arc(size_t i, size_t j) const
{
    auto ite = _arcs.find(ArcFromTo(i, j));
	return ite != _arcs.end();
}

const JobShopNode::Arc& JobShopNode::get_arc(size_t i, size_t j) const
{
    auto ite = _arcs.find(ArcFromTo(i, j));
	return ite->second;
}
