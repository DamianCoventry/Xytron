#include "stdafx.h"
#include "tree.h"
#include "../math/linesegment.h"
#include <stdexcept>

using namespace Bsp;

void Tree::Insert(const Math::Plane& plane, const Math::Vector& another_point_on_plane)
{
    if(nodes_.empty())
    {
        nodes_.push_back(NodePtr(new Node(unique_id_++, plane)));
    }
    else
    {
        Node* root = nodes_.front().get();
        Insert_Recursive(root, plane, another_point_on_plane);
    }
}

void Tree::Clear()
{
    nodes_.clear();
}

void Tree::Insert_Recursive(Node* n, const Math::Plane& plane, const Math::Vector& another_point_on_plane)
{
    Math::LineSegment ls(plane.on_plane_, another_point_on_plane);
    switch(n->plane_.ClassifyLineSegment(ls))
    {
    case Math::Plane::LSP_BEHIND:
        if(n->back_)
        {
            // Recurse down the back back
            Insert_Recursive(n->back_, plane, another_point_on_plane);
        }
        else
        {
            // Add node to the back tree
            NodePtr new_node(new Node(unique_id_++, plane));
            n->back_ = new_node.get();
            nodes_.push_back(new_node);
        }
        break;
    case Math::Plane::LSP_INFRONT:
        if(n->front_)
        {
            // Recurse down the front tree
            Insert_Recursive(n->front_, plane, another_point_on_plane);
        }
        else
        {
            // Add node to the front tree
            NodePtr new_node(new Node(unique_id_++, plane));
            n->front_ = new_node.get();
            nodes_.push_back(new_node);
        }
        break;
    case Math::Plane::LSP_ONPLANE:
        {
            float result = (float)fabs((n->plane_.normal_.x_ - plane.normal_.x_)+
                                       (n->plane_.normal_.y_ - plane.normal_.y_)+
                                       (n->plane_.normal_.z_ - plane.normal_.z_));
            // Is this plane facing the same way?
            if(result < 0.1f)
            {
                // This plane is already in the list.  Don't add it again.
            }
            else
            {
                // This plane is sharing the same plane, but is facing in the
                // opposite direction.
                if(n->back_)
                {
                    // Recurse down the back back
                    Insert_Recursive(n->back_, plane, another_point_on_plane);
                }
                else
                {
                    // Add node to the back tree
                    NodePtr new_node(new Node(unique_id_++, plane));
                    n->back_ = new_node.get();
                    nodes_.push_back(new_node);
                }
            }
            break;
        }
    case Math::Plane::LSP_SPANNING:
        throw std::runtime_error("Bsp::Tree::Insert - Source plane is spanning current plane, this is not supported.");
    }
}

bool Tree::IsLineOfSight(const Math::LineSegment& ls, float plane_shift) const
{
    Node* root = nodes_.front().get();
    return IsLineOfSight_Recursive(root, ls, plane_shift) == LOS_EMPTY;
}

Tree::LosReturn Tree::IsLineOfSight_Recursive(Node* n, const Math::LineSegment& ls, float plane_shift) const
{
    switch(n->plane_.ClassifyLineSegment(ls, plane_shift))
    {
    case Math::Plane::LSP_BEHIND:
        // This line segment is completely behind the plane.
        if(n->back_)
        {
            // There are back nodes, send the line segment down the back
            return IsLineOfSight_Recursive(n->back_, ls, plane_shift);
        }
        // There is solid space behind this plane
        return LOS_SOLID;
    case Math::Plane::LSP_INFRONT:
    case Math::Plane::LSP_ONPLANE:
        if(n->front_)
        {
            // There are front nodes, send the line segment down the front
            return IsLineOfSight_Recursive(n->front_, ls, plane_shift);
        }
        // There is empty space infront of this plane
        return LOS_EMPTY;
    }
    // else Math::Plane::LSP_SPANNING

    // This line segment is spanning this plane. Split it into two pieces and send
    // the front split down the front of the tree, and the back split down the back
    // of the tree.

    Math::Vector intersection;
    float split_percent;
    n->plane_.GetIntersection(ls, intersection, plane_shift, &split_percent);

    Math::LineSegment front_split(ls.begin_, intersection);
    Math::LineSegment back_split(intersection, ls.end_);

    if(n->front_)
    {
        // If the line segment started infront of the plane, then send the front split
        // down the front side of the plane.
        LosReturn los;
        if(n->plane_.ClassifyPoint(ls.begin_) != Math::Plane::PP_BEHIND)
        {
            los = IsLineOfSight_Recursive(n->front_, front_split, plane_shift);
        }
        else
        {
            los = IsLineOfSight_Recursive(n->front_, back_split, plane_shift);
        }

        if(los == LOS_SOLID)
        {
            // The split hit solid space, no point continuing.
            return los;
        }
        // else the split found empty space. So now test the other split.
    }

    if(n->back_)
    {
        // If the line segment started infront of the plane, then send the back split
        // down the back side of the plane.
        if(n->plane_.ClassifyPoint(ls.end_) == Math::Plane::PP_BEHIND)
        {
            return IsLineOfSight_Recursive(n->back_, back_split, plane_shift);
        }
        return IsLineOfSight_Recursive(n->back_, front_split, plane_shift);
    }

    // There is solid space behind this plane
    return LOS_SOLID;
}

Math::Vector Tree::TraceLineSegment_Slide(const Math::LineSegment& orig_ls, float plane_shift) const
{
    Math::LineSegment ls = orig_ls;
    Math::Vector forward(ls.end_ - ls.begin_);
    Math::Vector temp, new_end;
    Node* root = nodes_.front().get();

    TraceResult tr;
    TraceLineSegment_Recursive(root, ls, 0.0f, 1.0f, plane_shift, tr);

    for(int i = 0; i < 8 && tr.collided_; i++)
    {
        temp = tr.collision_plane_.on_plane_ - ls.end_;
        float dist_from_plane = temp.DotProduct(tr.collision_plane_.normal_); 

        new_end = ls.end_ + (tr.collision_plane_.normal_ * (dist_from_plane + plane_shift));
        ls      = Math::LineSegment(tr.collision_point_, new_end);

        TraceLineSegment_Recursive(root, ls, 0.0f, 1.0f, plane_shift, tr);
    }

    return tr.collision_point_;
}

void Tree::TraceLineSegment_Recursive(Node* n, const Math::LineSegment& ls, float begin_percent, float end_percent, float plane_shift, TraceResult& tr) const
{
    switch(n->plane_.ClassifyLineSegment(ls, plane_shift))
    {
    case Math::Plane::LSP_BEHIND:
        // This line segment is completely behind the plane.
        if(n->back_)
        {
            // There are back nodes, send the line segment down the back
            return TraceLineSegment_Recursive(n->back_, ls, begin_percent, end_percent, plane_shift, tr);
        }
        // There is solid space behind this plane
        tr.collided_        = true;
        tr.percent_         = begin_percent;
        tr.collision_point_ = ls.begin_;
        tr.collision_plane_ = n->plane_;
        return;
    case Math::Plane::LSP_INFRONT:
    case Math::Plane::LSP_ONPLANE:
        if(n->front_)
        {
            // There are front nodes, send the line segment down the front
            return TraceLineSegment_Recursive(n->front_, ls, begin_percent, end_percent, plane_shift, tr);
        }
        // There is empty space infront of this plane
        tr.collided_        = false;
        tr.percent_         = end_percent;
        tr.collision_point_ = ls.end_;
        tr.collision_plane_ = Math::Plane();
        return;
    }
    // else Math::Plane::LSP_SPANNING

    // This line segment is spanning this plane. Split it into two pieces and send
    // the front split down the front of the tree, and the back split down the back
    // of the tree.

    Math::Vector intersection;
    float split_percent;
    n->plane_.GetIntersection(ls, intersection, plane_shift, &split_percent);

    if(n->front_)
    {
        // Send the front split done the front side of the tree
        TraceLineSegment_Recursive(n->front_, Math::LineSegment(ls.begin_, intersection), begin_percent, split_percent, plane_shift, tr);
        if(tr.collided_)
        {
            // The front split hit solid space, no point testing the back split.
            return;
        }
        // else the front split found empty space. So now test the back split.
    }

    if(n->back_)
    {
        // Send the back split down the back side of the tree
        TraceLineSegment_Recursive(n->back_, Math::LineSegment(intersection, ls.end_), split_percent, end_percent, plane_shift, tr);
        if(!tr.collided_)
        {
            // The back split didn't collide with anything.
            return;
        }
        // Else make the collision where this split occured.
    }

    // There is solid space behind this plane
    tr.collided_        = true;
    tr.percent_         = split_percent;
    tr.collision_point_ = ls.begin_;
    tr.collision_plane_ = n->plane_;
}
