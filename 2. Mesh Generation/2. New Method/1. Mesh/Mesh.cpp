#include "Mesh.h"

// Initial_Mesh data input
void Mesh::Input()
{	
	std::ifstream ist;
	ist.open("InitialMesh.txt");
	std::string ss;	

	// input Points
	std::getline(ist, ss);
	size_t np{ 0 }; ist >> np;
	for (size_t i = 0; i < np; i++)
	{
		static size_t n{ 0 };
		static double x{ 0.0 }, y{ 0.0 }, r{ 0.0 };
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
		ist >> n >> a >> b >> c ;
		ts.emplace_back(a, b, c);
	}
	ist >> std::ws;	

	ist.close();
}

void Mesh::Output() const
{
	//const std::string name{ "Mesh.txt" };

	std::ofstream ost;
	ost.open("Mesh.txt");

	// output Points 
	ost << "Points\n";
	ost << ps.size() << '\n';
	for (std::size_t i = 1; auto & p : ps)
	{
		ost << std::scientific << i << '\t'
			<< p.x << '\t' << p.y << '\t'
			<< p.r << '\t'  << '\n';
		i++;
	}

	// output Triangles 
	ost << "Triangles\n";
	ost << ts.size() << '\n';
	for (std::size_t i = 1; auto & t : ts)
	{
		ost << std::scientific << i << '\t'
			<< t.a << '\t' << t.b << '\t' << t.c << '\t' <<  '\n';
		i++;
	}

	ost.close();
}



void Mesh::InsertTriange(std::size_t n)
{
	for (auto&& [e, c] : ts[n].ec3)
	{
		ps[c].es.insert(e);		
		
		auto& ep = eeps[e];
		switch (ep.sz++)
		{
		case 0:
		{ auto [a, b] {e};
		ps[a].nps.insert(b);
		ps[b].nps.insert(a); }
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
}


void Mesh::EraseTriange(std::size_t n)
{
	for (auto&& [e, c] : ts[n].ec3)
	{
		ps[c].es.erase(e);		
		
		auto it = eeps.find(e);
		if (it != eeps.end())
		{
			auto& ep{ it->second };

			switch (ep.sz)
			{
			case 1:
			{ auto [a, b] {e};
			ps[a].nps.erase(b);
			ps[b].nps.erase(a); }
				eeps.erase(it);
				break;
			case 2:
				if (ep.n1 == n)
				{
					ep.n1 = ep.n2;
					ep.c1 = ep.c2;
				}
				ep.sz--;
				break;
			default:
				std::cerr << "eeps erase error! \n";
				break;
			}
		}
	}
}

// mesh data initialization. 
void Mesh::Initialization()
{
	// eepss initialization
	for (std::size_t i = 0; auto & t : ts)
	{
		InsertTriange(i);
		i++;
	}

	//ps initialization
	for (auto& p : ps)
		p.r *= 1.3;

	for (auto& [e, ep] : eeps)
	{
		if (ep.sz == 1)
		{
			ps[e.a].isatboundary = true;
			ps[e.b].isatboundary = true;
		}
	}

/*	for (auto & t : ts)
	{		
		auto& pa{ ps[t.a] }; auto& pb{ ps[t.b] }; auto& pc{ ps[t.c] };
		double xa{ pa.x }, ya{ pa.y }, xb{ pb.x }, yb{ pb.y }, xc{ pc.x }, yc{ pc.y };
		AREA += std::abs((yb - yc) * (xa - xc) - (xb - xc) * (ya - yc)) / 2.0;
	}

	for (auto& p : ps)	
		area += PI * std::pow(p.r, 2);	*/	

	//output();	
}



auto Mesh::GetMaxEdgeAndRelativeLength()
{	
	Edge emax;
	double rlmax = 0.0;

	for (auto& [e,ep]: eeps)
	{
		if (ep.sz == 2)
		{					
			auto& pa{ ps[e.a] }; auto& pb{ ps[e.b] };
			double rl= std::hypot(pb.x - pa.x, pb.y - pa.y) / (pa.r + pb.r) ;
			if (rlmax < rl)
			{
				rlmax = rl;
				emax = e;
			}
		}
	}

	return std::make_pair(emax,rlmax);
}



// mesh data initialization. 
void Mesh::InsertBubbleAtMiddleEdge(Edge& e)
{
	EdgeP& ep = eeps.at(e);
	auto [a, b] {e};
	std::size_t n1{ ep.n1 }; std::size_t c1{ ep.c1 };
	std::size_t n2{ ep.n2 }; std::size_t c2{ ep.c2 };
	auto& pa{ ps[a] }; auto& pb{ ps[b] };

	double x = (pa.x + pb.x) / 2.0;
	double y = (pa.y + pb.y) / 2.0;
	double r = (pa.r + pb.r) / 2.0;

	//delete old data
	EraseTriange(n1);	
	EraseTriange(n2); 

	//update new data
	ps.emplace_back(x, y, r);
	std::size_t n{ ps.size() - 1 };

	ts[n1] = Triangle(a, c1, n); InsertTriange(n1);
	ts[n2] = Triangle(a, c2, n); InsertTriange(n2);
	ts.emplace_back(b, c1, n); InsertTriange(ts.size()-1);
	ts.emplace_back(b, c2, n); InsertTriange(ts.size() - 1);

}

void Mesh::UpdateBubblesPositions()
{
	for (auto& p : ps)
	{
		if (p.isatboundary == false)
		{	
			double dx = 0.0, dy = 0.0;
			for (auto& a : p.nps)
			{
				auto& pa{ ps[a] };
				double length = std::hypot(pa.x - p.x, pa.y - p.y);
				double r = p.r + pa.r;

				double f = (length > r) ? 0.0 : (length - r);
				double nx = (pa.x - p.x) / length;
				double ny = (pa.y - p.y) / length;
				dx += 0.3 * f * nx;
				dy += 0.3 * f * ny;
			}

			double& x{ p.x }; double& y{ p.y }; 			
			double area{ 0.0 };
			for (auto& e : p.es)
			{				
				auto& pa{ ps[e.a] }; auto& pb{ ps[e.b] };
				double xa{ pa.x }, ya{ pa.y }, xb{ pb.x }, yb{ pb.y };
				area += std::abs((yb - y) * (xa - x) - (xb - x) * (ya - y))/2.0;
			}

			double xn{ x+ dx }; double yn{ y + dy }; 
			bool isInCavity{ false };
			while (isInCavity == false)
			{				
				double areaNew{ 0.0 };
				for (auto& e : p.es)
				{
					auto& pa{ ps[e.a] }; auto& pb{ ps[e.b] };
					double xa{ pa.x }, ya{ pa.y }, xb{ pb.x }, yb{ pb.y };
					areaNew += std::abs((yb - yn) * (xa - xn) - (xb - xn) * (ya - yn)) / 2.0;
				}

				if ( (areaNew - area) / area > 1e-8)
				{
					xn = (x + xn) / 2.0;
					yn = (y + yn) / 2.0;
				}
				else
					isInCavity = true;
			}			
			x = xn; y = yn;
		}
	}
}


double Mesh::GetTheta(std::size_t a, std::size_t b, std::size_t c1, std::size_t c2) const
{
	auto& pa{ ps[a] }; auto& pb{ ps[b] };
	auto& pc1{ ps[c1] }; auto& pc2{ ps[c2] };

	double xa{ pa.x }, ya{ pa.y }, xb{ pb.x },  yb{ pb.y };
	double xc1{ pc1.x }, yc1{ pc1.y }, xc2{ pc2.x }, yc2{ pc2.y };
	double xc1a{ xa - xc1 }, yc1a{ ya - yc1 }, xc1b{ xb - xc1 }, yc1b{ yb - yc1 },
		xc2a{ xa - xc2 }, yc2a{ ya - yc2 }, xc2b{ xb - xc2 }, yc2b{ yb - yc2 };

	double theta1 = std::acos((xc1a * xc1b + yc1a * yc1b)
		/ (std::hypot(xc1a, yc1a) * std::hypot(xc1b, yc1b)));
	double theta2 = std::acos((xc2a * xc2b + yc2a * yc2b)
		/ (std::hypot(xc2a, yc2a) * std::hypot(xc2b, yc2b)));

	return 2.0 * PI - theta1 - theta2;
}



void Mesh::UpdatePointsConnections()
{
	using ThetaEs = std::multimap<double, const Edge>;
	using EIts = std::unordered_map<Edge, ThetaEs::iterator, EdgeHash, EdgeEqual>;
	ThetaEs thetaes;  EIts eits;

	for (auto& [e, ep] : eeps)
	{
		if (ep.sz == 2)
		{
			double theta = GetTheta(e.a, e.b, ep.c1, ep.c2);
			if (theta < PI - 1e-8)
			{
				auto it = thetaes.emplace(theta, e);
				eits.emplace(e, it);
			}
		}
	}

	size_t i = 0;	
	while (thetaes.size() > 0)
	{
		auto itmin = thetaes.begin();
		auto& emin{ itmin->second };	auto& epmin = eeps.at(emin);
		auto [a, b] {emin};
		auto n1{ epmin.n1 }; auto n2{ epmin.n2 };
		auto c1{ epmin.c1 }; auto c2{ epmin.c2 };

		//delete olds 				
		EraseTriange(n1);
		EraseTriange(n2);

		std::unordered_set<Edge, EdgeHash, EdgeEqual> eolds;
		for (auto n : { n1,n2 })
			for (auto& [eold, cold] : ts[n].ec3)
				eolds.emplace(eold);

		for (auto& eold : eolds)
		{
			auto iteit = eits.find(eold);
			if (iteit != eits.end())
			{
				thetaes.erase(iteit->second);
				eits.erase(iteit);
			}
		}

		//add news
		ts[n1] = Triangle(c1, c2, a);
		ts[n2] = Triangle(c1, c2, b);

		InsertTriange(n1);
		InsertTriange(n2);


		std::unordered_set<Edge, EdgeHash, EdgeEqual> enews;
		for (auto n : { n1,n2 })
			for (auto& [enew, cnew] : ts[n].ec3)
				enews.emplace(enew);

		for (auto& enew : enews)
		{
			auto& epnew = eeps.at(enew);
			if (epnew.sz == 2)
			{
				double theta = GetTheta(enew.a, enew.b, epnew.c1, epnew.c2);
				if (theta < PI - 1e-8)
				{
					auto itnew = thetaes.emplace(theta, enew);
					eits.emplace(enew, itnew);
				}
			}
		}

		//if ((i++) % (20) == 0)
		//{
		//	Output();
		//	continue;
		//}
		
	}
}



void Mesh::UpdateBubblesRadii()
{
	for (auto&& p : ps)
	{
		if (p.isatboundary == false)
		{
			double sum_r{ 0.0 }, sum{ 0.0 };
			for (auto& a : p.nps)
			{
				auto& pa{ ps[a] };
				auto length = std::hypot(pa.x - p.x, pa.y - p.y);

				sum_r += pa.r / length;
				sum += 1.0 / length;

				//double r = p.r + pa.r;
				//if (length < r)
				//{
				//	sum_r += pa.r / length;
				//	sum += 1.0 / length;
				//}
			}

			//p.r += 0.5*(sum_r / sum - p.r);
			p.r = sum_r / sum ;
		}
	}

	//for (auto&& p : ps)
	//{
	//	if (p.isatboundary == false)
	//		p.r += p.dr;
	//}
}


void Mesh::MeshGeneration()
{	
	//Output();
	for (std::size_t ii = 0; ; ii++)
	{		
		auto [emax, rlmax] = GetMaxEdgeAndRelativeLength();				
		if (rlmax > 1)
		{
			InsertBubbleAtMiddleEdge(emax);
			//Output();

			UpdatePointsConnections();
			//Output();

			UpdateBubblesRadii();
			//Output();

			if (rlmax < 1.3)
			{
				//Output();
				for (std::size_t i = 0; i < 30; i++)
				{
					UpdateBubblesPositions();
					//Output();

					UpdatePointsConnections();
					//Output();

					UpdateBubblesRadii();
					//Output();
				}
				//Output();
			}
					
			
			//if (ii % (20) == 0)
			//{
			//	Output();
			//	continue;
			//}

		}
			
		else
		{			
			//Output();
			for (std::size_t i = 0; i < 500; i++)
			{
				UpdateBubblesPositions();
				//Output();
				UpdatePointsConnections();
				//Output();
				UpdateBubblesRadii();
				//Output();
			}

			break;
		}			
	}
}
