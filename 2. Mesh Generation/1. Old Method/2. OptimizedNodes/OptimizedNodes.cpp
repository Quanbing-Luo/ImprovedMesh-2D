#include "OptimizedNodes.h"


// mesh data input 
void OptimizedNodes::Input()
{
	std::ifstream ist;
	ist.open("Mesh.txt");
	std::string ss;	

	// input Points
	std::getline(ist, ss);
	size_t np{ 0 }; ist >> np;
	for (size_t i = 0; i < np; i++)
	{
		static size_t n{ 0 }; 	 
		static double x{ 0.0 }, y{ 0.0 }, r{0.0};
		ist >> n >> x >> y >> r;
		ps.emplace_back(x, y, r);
	}
	ist >> std::ws;


	// input Triangles 
	std::getline(ist, ss);
	size_t nt{ 0 }; ist >> nt;	
	for (size_t i = 0; i < nt; i++)
	{
		static size_t n{ 0 };
		static size_t a{ 0 }, b{ 0 }, c{ 0 };
		ist >> n >> a >> b >> c;
		ts.emplace_back(a, b, c);
	}
	ist >> std::ws;

	ist.close();
}


void OptimizedNodes::Output() const
{
	const std::string name{ "OptimizedNodes.txt" };
	std::ofstream ost;
	ost.open(name);
	// output Points 
	ost << "Points\n";
	ost << ps.size() << '\n';
	for (size_t i = 1; auto & p : ps)
	{
		ost << std::scientific << i << '\t'
			<< p.x << '\t' << p.y << '\t' << p.r << '\n';
		i++;
	}

	// output Triangles 
	ost << "Triangles\n";
	ost << ts.size() << '\n';
	for (size_t i = 1; auto & t : ts)
	{
		ost << std::scientific << i << '\t'
			<< t.a << '\t' << t.b << '\t' << t.c << '\t' << '\n';
		i++;
	}

	// output Edges 
	ost << "Edges\n";
	size_t n = 0;
	for ( auto & [e, ep] : eeps)
	{
		if (ep.sz == 1)		
			n++;		
	}
	ost << n << '\n';
	for (size_t i = 1; auto & [e,ep] : eeps)
	{
		if (ep.sz == 1)
		{
			ost << std::scientific << i << '\t'
				<< e.a << '\t' << e.b << '\t'  << '\n';
			i++;
		}
	}

	ost.close();
}





std::vector<size_t>  OptimizedNodes::GetAroundPoints(std::size_t n, std::size_t LEVEL)
{
	auto aphs{ ps[n].nps };
	std::vector<size_t> aps; 
	for (auto& nn : aphs) aps.push_back(nn);
	aphs.insert(n);

	std::size_t ibegin{ 0 }; std::size_t iend{ aps.size()};
	for (std::size_t level = 0; level < LEVEL; level++)
	{ 
		for (std::size_t i = ibegin; i < iend; i++)
		{
			for (auto nn : ps[aps[i]].nps)
			{
				auto [it, is] = aphs.insert(nn);
				if (is == true)
					aps.push_back(nn);
			}
		}
		ibegin = iend; iend = aps.size();
	}

	return aps;
}


//data_initialization
void OptimizedNodes::Initialization()
{
	//eepss initialization
	for (std::size_t n=0; auto& t : ts)
	{
		for (auto&& [e, c] : ts[n].ec3)
		{
			auto& ep = eeps[e];
			switch (ep.sz++)
			{
			case 0:
				ep.n1 = n;	ep.c1 = c;
				//ep.length = std::hypot(ps[e.b].x - ps[e.a].x, ps[e.b].y - ps[e.a].y);
				break;
			case 1:
				ep.n2 = n; 	ep.c2 = c;
				break;
			default:
				std::cerr << "eeps insert error! \n";
				break;
			}
		}
		n++;
	}
		
	//Points initialization (set radii) 
	for (auto& p : ps)	
		p.r *= 1.7;	

	for (auto& [e, ep] : eeps)
	{
		auto& [a, b] {e};
		ps[a].nps.insert(b);
		ps[b].nps.insert(a);		
		
		if (ep.sz == 1)
		{
			ps[a].isatboundary = true;
			ps[b].isatboundary = true;
		}
	}


	for (std::size_t i = 0; auto & p : ps)
	{
		p.aps = GetAroundPoints(i, 3);
		i++;
	}
	//output();
}



void OptimizedNodes::UpdateBubblesPositions()
{
	for (auto& p : ps)
	{
		if (p.isatboundary == false)
		{
			auto& dx = p.dx; dx = 0.0;
			auto& dy = p.dy; dy = 0.0;
			for (auto& a : p.aps)
			{
				auto& pa{ ps[a] };
				double length = std::hypot(pa.x - p.x, pa.y - p.y);
				double r = p.r + pa.r;
				
				double f = (length > r) ? 0.0 : (length - r);
				double nx = (pa.x - p.x) / length;
				double ny = (pa.y - p.y) / length;
				dx += 0.1 * f * nx;
				dy += 0.1 * f * ny;
			}			
		}
	}

	for (auto& p : ps)
	{
		if (p.isatboundary == false)
		{
			p.x += p.dx;
			p.y += p.dy;
		}		
	}
}


void OptimizedNodes::UpdateBubblesRadii()
{
	for (auto&& p : ps)
	{
		if (p.isatboundary == false)
		{		
			double sum_r{ 0.0 }, sum{ 0.0 };
			for (auto& a : p.aps)
			{
				auto& pa{ ps[a] };
				auto length = std::hypot(pa.x - p.x, pa.y - p.y);
				double r = p.r + pa.r;
				if (length < r)
				{
					sum_r += pa.r / length;
					sum += 1.0 / length;
				}
			}

			p.dr = 0.5 *(sum_r / sum - p.r);
		}
	}

	for (auto&& p : ps)
	{
		if (p.isatboundary==false)			
		p.r+= p.dr;
	}
}



void OptimizedNodes::Optimization()
{
	//double r_max{ 0.0 };

	//size_t NN {10 * static_cast<size_t> (std::sqrt(ps.size())) };
	size_t NN {10* static_cast<size_t> (std::sqrt(ps.size())) };
	for (size_t i = 0; i < NN; i++)
	{
		//for (size_t j = 0; j < 20; j++)
			UpdateBubblesPositions();
		UpdateBubblesRadii();
		
		//if (i % 10 == 0)
		//{
		//	Output();
		//	continue;
		//}

	}

	//Output();
	//UpdatePointsConnections();

}

