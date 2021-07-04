#ifndef INCLUDED_BSP_TREE
#define INCLUDED_BSP_TREE

#include "../math/plane.h"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace Bsp
{

class Tree
{
public:
    Tree() : unique_id_(0) {}

    void Insert(const Math::Plane& plane, const Math::Vector& another_point_on_plane);
    void Clear();

    bool IsLineOfSight(const Math::LineSegment& ls, float plane_shift) const;
    Math::Vector TraceLineSegment_Slide(const Math::LineSegment& ls, float plane_shift) const;
    // TODO: Math::Vector TraceLineSegment_Reflect(const Math::LineSegment& ls, float plane_shift) const;

private:
    struct Node
    {
        Node(int id) : id_(id), front_(NULL), back_(NULL) {}
        Node(int id, const Math::Plane& p) : id_(id), front_(NULL), back_(NULL), plane_(p) {}
        int id_;
        Node* front_;
        Node* back_;
        Math::Plane plane_;
    };
    typedef boost::shared_ptr<Node> NodePtr;
    typedef std::vector<NodePtr> Nodes;
    Nodes nodes_;
    int unique_id_;
private:
    void Insert_Recursive(Node* n, const Math::Plane& plane, const Math::Vector& another_point_on_plane);

    enum LosReturn { LOS_SOLID, LOS_EMPTY };
    LosReturn IsLineOfSight_Recursive(Node* n, const Math::LineSegment& ls, float plane_shift) const;

    struct TraceResult
    {
        TraceResult() : collided_(false), percent_(0.0f) {}
        bool collided_;
        float percent_;
        Math::Vector collision_point_;
        Math::Plane collision_plane_;
    };
    void TraceLineSegment_Recursive(Node* n, const Math::LineSegment& ls, float begin_percent, float end_percent, float plane_shift, TraceResult& tr) const;
};

}       // namespace Bsp

#endif  // INCLUDED_BSP_TREE
