/**
 * GLAROT-3D - Geometric LAndmark relations ROTation-invariant 3D 
 * Copyright (C) 2017 Dario Lodi Rizzini.
 *
 * glarot3d is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * glarot3d is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with glarot3d.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <Eigen/Dense>
#include <iostream>

namespace glarot3d
{

/** This class implements the Combined Constraint Data Association (CCDA)
 * proposed by Tim Bailey. The best reference is:
 *
 * T. Bailey, "Mobile Robot Localisation and Mapping in Extensive Outdoor Environments", 
 *  PhD Thesis, University of Sydney, 2002.
 *
 * Of course, there are the related papers. 
 * The method is based on the so called CorrespondenceGraph built on two point sets.
 * Data Association is computed as the maximum clique CorrespondenceGraph. 
 *
 * Maximum clique has been computed according to T. Bailey suggestions.
 * However, for different approaches see:
 *  http://www.dharwadker.org/clique/
 *  http://www.sanfoundry.com/cpp-program-find-maximum-size-clique-graph/
 *  http://www.sicmm.org/~konc/maxclique/mcqd/mcqd.h
 */
class CorrespondenceGraph
{
public:
  typedef Eigen::Vector3d Point;
  typedef Eigen::Matrix3d Matrix;
  typedef Eigen::Affine3d Transformation;

  /** Node of correspondence graph represents an association 
   * between an input point and a target point. 
   */
  struct Node 
  {
    int index;      // global index (that should correspond to position in nodes vector)
    int inputId;    // input point ID
    int targetId;   // target point ID
    std::vector<int> adjacents;

    bool operator<(const Node& n) const { return (index < n.index); }
    int degree() const { return adjacents.size(); }
  };
  
  /** Distance constraint between the point belonging to the same set.
   */ 
  struct Constraint
  {
    int i;
    int j;
    double dist;

    bool operator<(const Constraint& c) const { return (dist < c.dist); }
  };

  /** Creates a new empty correspondence graph. 
   */
  CorrespondenceGraph();

  /** Sets the target points.
   */
  void setTargets(const std::vector<Point>& targets);

  /** Sets the target points.
   */
  void setInputs(const std::vector<Point>& inputs);
 
  /** Sets the distance tollerance used in pair associations.
   */
  void setDistanceToll(double dt) { distToll_ = dt; }

  /** Sets the minimum intra-distance to avoid graph explosion. 
   */
  void setDistanceMin(double dm) { distMin_ = dm; }

  /** Computes the associations.
   */
  void associate(std::vector<std::pair<int,int> >& associations);

  /** Computes the transformation that maps input points to target points. 
   */
  void computeTransform(Transformation& transf,std::vector<std::pair<int,int> >& associations);

  /** Prints the correspondence graph.
   */
  void printGraph(std::ostream& out);

  /** Prints the constraints.
   */
  void printConstraints(std::ostream& out);

private:
  std::vector<Node> nodes_;
  std::vector<std::pair<int,int> > edges_;
  std::vector<Point> targets_;
  std::vector<Point> inputs_;
  std::vector<Constraint> constraintsInput_;
  std::vector<Constraint> constraintsTarget_;
  double distToll_;
  double distMin_;

  /** Creates node set.
   */
  void makeNodeSet(const std::vector<Point>& points1,const std::vector<Point>& points2,
                    std::vector<Node>& nodes);

  /** Creates the list of constraints.
   */
  void makeRelativeConstraints(const std::vector<Point>& points,std::vector<Constraint>& constraints);

  /** Sorts the adjacent list of each nodes according to their degree.
   */
  void sortAdjacent(std::vector<Node>& nodes);

  void findCliqueBronKerbosch(const std::vector<Node>& nodes,
    const std::vector<int>& clique,const std::vector<int>& candidates,const std::vector<int>& visited,
    std::vector<int>& cliqueMax);

  /** Depth-first clique search.
   */
  void findClique(const std::vector<Node>& nodes,std::vector<int>& cliqueMax);

  /** Recursive graph search...
   */
  void findCliqueRecursive(const std::vector<Node>& nodes,int id,
    std::vector<int>& cliqueCur,std::vector<int>& cliqueMax, std::vector<int>& mutuallyAdjacents);

  /** Updates the mutually adjacent set by intersecting the adjacent nodes of id
   * and the given mutually adjacent set. 
   */
  void updateMutuallyAdjacents(const std::vector<Node>& nodes,int id,std::vector<int>& mutuallyAdjacents);

  /** Returns a random index among the passed id vector.
   */
  int getRandomId(const std::vector<int>& ids);

  /**
   */
  void findCliqueDyn(const std::vector<Node>& nodes,std::vector<int>& cliqueMax);

  /** Computes the transformation from points1 to points2. 
   */
  bool computeTransform(const std::vector<Point>& points1,const std::vector<Point>& points2,
                        const std::vector<std::pair<int,int> >& indices,Transformation& transform);
};

} // end of namespace glarot3d

