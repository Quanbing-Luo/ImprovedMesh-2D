#include "Mesh.h"

// Initial_Mesh data input
void Mesh::input()
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
		ps.emplace_back(x, y, 0.0);
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

void Mesh::output() const
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


void Mesh::Triangle::setParameters(const Points& ps)
{
	auto& pa{ ps[a] }; auto& xa{ pa.x }; auto& ya{ pa.y }; auto& ra{ pa.r };
	auto& pb{ ps[b] }; auto& xb{ pb.x }; auto& yb{ pb.y }; auto& rb{ pb.r };
	auto& pc{ ps[c] }; auto& xc{ pc.x }; auto& yc{ pc.y }; auto& rc{ pc.r };

	double  temp{ (xb - xa) * (yc - ya) - (xc - xa) * (yb - ya) };
	double  temp1{ (pow(xb, 2) - pow(xa, 2)) + (pow(yb, 2) - pow(ya, 2)) };
	double  temp2{ (pow(xc, 2) - pow(xa, 2)) + (pow(yc, 2) - pow(ya, 2)) };

	// circumcenter and radius of Circumcircle
	xcc = (temp1 * (yc - ya) - temp2 * (yb - ya)) / (2.0 * temp);
	ycc = (temp1 * (xc - xa) - temp2 * (xb - xa)) / (-2.0 * temp);
	rcc = hypot(xcc - xa, ycc - ya);

	//the center of gravity
	x = (xa + xb + xc) / 3.0;
	y = (ya + yb + yc) / 3.0;

	double lag{ std::hypot(x - xa, y - ya) },
		lbg{ std::hypot(x - xb, y - yb) }, lcg{ std::hypot(x - xc, y - yc) };
	r = (ra / lag + rb / lbg + rc / lcg) / (1.0 / lag + 1.0 / lbg + 1.0 / lcg);

	area = abs(temp) / 2.0;
	area_bubble = PI * std::pow(r, 2);
	density = area_bubble / area;
}


void Mesh::insertTriangeEdges(std::size_t n)
{
	for (auto&& [e, c] : ts[n].ec3)
	{
		auto& ep = eeps[e];
		switch (ep.sz++)
		{
		case 0:
			ep.n1 = n;	ep.c1 = c;
			ep.length = std::hypot(ps[e.b].x - ps[e.a].x, ps[e.b].y - ps[e.a].y);
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


void Mesh::eraseTriangeEdges(std::size_t n)
{
	for (auto&& [e, c] : ts[n].ec3)
	{
		auto it = eeps.find(e);
		if (it != eeps.end())
		{
			auto& ep{ it->second };

			switch (ep.sz)
			{
			case 1:
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
void Mesh::initialization()
{
	// eepss initialization
	for (std::size_t i = 0; auto & t : ts)
	{
		insertTriangeEdges(i);
		i++;
	}

	//ps initialization
	for (auto& [e, ep] : eeps)
	{
		auto& [a, b] {e}; auto& pa{ ps[a] }; auto& pb{ ps[b] };
		if (ep.sz == 1)
		{
			double half_area{ std::pow(ep.length , 2.0) * std::sqrt(3.0) / 8.0 };
			pa.r += half_area;	pb.r += half_area;			
		}
	}

	for (auto& p : ps)
		p.r = std::sqrt(p.r / PI);


	//ts initialization
	for (auto& t : ts)
		t.setParameters(ps);



	//output();	
}


Mesh::CavityPair  Mesh::getCavity(std::size_t nt) const
{	
	CavityPair cp;
	auto& [ns_cavity, es_cavity] {cp};

	ns_cavity.insert(nt);
	std::vector<std::size_t> ns{ nt };

	auto& t{ ts[nt] };	double x{ t.x}, y{t.y};	
	for (size_t i = 0; i < ns.size(); i++)
	{
		auto& n{ ns[i] };	auto& ti{ ts[n] };
		for (auto&& [e, c] : ti.ec3)
		{			
			auto& ep = eeps.at(e);
			if (ep.sz == 2)
			{
				std::size_t nn = (ep.n1 == n) ? (ep.n2) : (ep.n1);
						
				if (ns_cavity.contains(nn) == false)
				{
					auto& tnn{ ts[nn] };
					if (hypot(tnn.xcc - x, tnn.ycc - y) < tnn.rcc)
					{
						ns_cavity.insert(nn);
						ns.push_back(nn);
					}
				}				
			}			
		}
	}
	

	for (auto&& n : ns_cavity)
		for (auto&& [e, c] : ts[n].ec3)
		{
			auto [it, is] = es_cavity.emplace(e);
			if (is == false) es_cavity.erase(it);
		}

	return cp;
}





void Mesh::meshGeneration()
{	
	double mesh_area{ 0.0 };
	double mesh_area_bubbles{ 0.0 };
	
	// mesh area initialization
	for (std::size_t nt = 0; auto & t:ts)
	{
		mesh_area += t.area;
		mesh_area_bubbles += t.area_bubble;
		nt++;
	}	

	for (std::size_t ii = 0; mesh_area_bubbles < mesh_area; ii++)
	{
		auto compare = [](const Triangle& ta, const Triangle& tb) { return ta.density < tb.density; };
		auto it = std::ranges::min_element(ts, compare);
		auto nt = std::ranges::distance(ts.begin(), it);

		auto& t{ ts[nt] };
		auto [ns_cavity, es_cavity] = getCavity(nt);

		//delete old data
		for (auto& n : ns_cavity)
		{
			mesh_area_bubbles -= ts[n].area_bubble;
			eraseTriangeEdges(n);
		}

		//update new data
		ps.emplace_back(t.x, t.y, t.r);
		std::size_t np{ ps.size() - 1 };
		for (auto it = ns_cavity.begin(); auto & e : es_cavity)
		{
			
			std::size_t nn{0};			
			if (it != ns_cavity.end())
			{
				nn = *it ;				
				ts[nn] = Triangle(e.a, e.b, np);
				it++;
			}			
			else
			{ 
				nn = ts.size();
				ts.emplace_back(e.a, e.b, np);
			}

			auto& tnn = ts[nn];
			tnn.setParameters(ps);
			mesh_area_bubbles += tnn.area_bubble;
			insertTriangeEdges(nn);		
		}
	}
}
