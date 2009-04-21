/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-02-13
 * Description : Layouting photos on a page
 *
 * Copyright 2007-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// C++ includes.

#include <cmath>

// Qt includes.

#include <QList>

// Local includes.

#include "layouttree.h"

namespace KIPIPrintImagesPlugin
{

/*
    Some comments refer to the paper mentioned in the header file
*/

LayoutNode::LayoutNode(double aspectRatio, double relativeArea, int index)
          : m_a(aspectRatio), m_e(relativeArea), m_division(0),
            m_type(TerminalNode), m_index(index),
            m_leftChild(0), m_rightChild(0)
{
}

LayoutNode::LayoutNode(LayoutNode *subtree, LayoutNode *terminalChild, bool horizontal, int index)
          : m_a(0), m_e(0), m_division(0),
            m_type(horizontal ? HorizontalDivision : VerticalDivision), m_index(index),
            m_leftChild(subtree), m_rightChild(terminalChild)
{
}

LayoutNode::LayoutNode(const LayoutNode &other)
{
    (*this) = other;
}

LayoutNode::~LayoutNode()
{
    delete m_leftChild;
    delete m_rightChild;
}

LayoutNode &LayoutNode::operator=(const LayoutNode &other)
{
    m_a = other.m_a;
    m_e = other.m_e;
    m_division = other.m_division;
    m_type = other.m_type;
    m_index = other.m_index;
    m_leftChild  = other.m_leftChild ? new LayoutNode(*other.m_leftChild) : 0;
    m_rightChild = other.m_rightChild ? new LayoutNode(*other.m_rightChild) : 0;

    return *this;
}

// replace one child with a new one
void LayoutNode::takeAndSetChild(LayoutNode *oldChild, LayoutNode *newChild)
{
    if (m_leftChild == oldChild)
        m_leftChild = newChild;
    else if (m_rightChild == oldChild)
        m_rightChild = newChild;
}

// retrieve the node which has the given index in the hierarchy of this node
LayoutNode *LayoutNode::nodeForIndex(int index)
{
    if (m_index == index)
        return this;

    if (m_type == TerminalNode)
        return 0;

    LayoutNode *fromLeft = m_leftChild->nodeForIndex(index);
    if (fromLeft)
        return fromLeft;
    return m_rightChild->nodeForIndex(index);
}

// retrieve the parent node of the given child in the hierarchy of this node
LayoutNode *LayoutNode::parentOf(LayoutNode *child)
{
    if (m_type == TerminalNode)
        return 0;

    if (m_leftChild == child || m_rightChild == child)
        return this;

    LayoutNode *fromLeft = m_leftChild->parentOf(child);
    if (fromLeft)
        return fromLeft;

    return m_rightChild->parentOf(child);
}

// compute the "aspect ratio" (a) and "relative size" (e) parameters
// Section 2.2.1, (1)-(4)
void LayoutNode::computeRelativeSizes()
{
    if (m_type == TerminalNode)
        return;

    m_leftChild->computeRelativeSizes();
    m_rightChild->computeRelativeSizes();

    double leftProductRoot = sqrt(m_leftChild->m_a * m_leftChild->m_e);
    double rightProductRoot = sqrt(m_rightChild->m_a * m_rightChild->m_e);
    double maxProductRoot = leftProductRoot > rightProductRoot ? leftProductRoot : rightProductRoot;

    double leftDivisionRoot = sqrt(m_leftChild->m_e / m_leftChild->m_a);
    double rightDivisionRoot = sqrt(m_rightChild->m_e / m_rightChild->m_a);
    double maxDivisionRoot = leftDivisionRoot > rightDivisionRoot ? leftDivisionRoot : rightDivisionRoot;

    if (m_type == VerticalDivision) // side by side
    {
        m_a = maxProductRoot / (leftDivisionRoot + rightDivisionRoot);
        m_e = maxProductRoot * (leftDivisionRoot + rightDivisionRoot);
    }
    else if (m_type == HorizontalDivision) // one on top of the other
    {
        m_a = (leftProductRoot + rightProductRoot) / maxDivisionRoot;
        m_e = maxDivisionRoot * (leftProductRoot + rightProductRoot);
    }
}

// Section 2.2.2
void LayoutNode::computeDivisions()
{
    if (m_type == TerminalNode)
        return;

    m_leftChild->computeDivisions();
    m_rightChild->computeDivisions();

    if (m_type == VerticalDivision) // side by side
    {
        double leftDivisionRoot = sqrt(m_leftChild->m_e / m_leftChild->m_a);
        double rightDivisionRoot = sqrt(m_rightChild->m_e / m_rightChild->m_a);

        m_division = leftDivisionRoot / (leftDivisionRoot + rightDivisionRoot);
    }
    else if (m_type == HorizontalDivision) // one on top of the other
    {
        // left child is topmost
        double leftProductRoot = sqrt(m_leftChild->m_a * m_leftChild->m_e);
        double rightProductRoot = sqrt(m_rightChild->m_a * m_rightChild->m_e);

        // the term in the paper takes 0 = bottom, we use 0 = top
        m_division = 1 - (rightProductRoot / (rightProductRoot + leftProductRoot));
    }
}

// --------------------------------------------- //

LayoutTree::LayoutTree(double aspectRatioPage, double absoluteAreaPage)
          : m_root(0), m_count(0), m_aspectRatioPage(aspectRatioPage), 
            m_absoluteAreaPage(absoluteAreaPage)
{
}

LayoutTree::LayoutTree(const LayoutTree &other)
{
    *this = other;
}

LayoutTree &LayoutTree::operator=(const LayoutTree &other)
{
    m_root = new LayoutNode(*(other.m_root));
    m_count = other.m_count;
    m_aspectRatioPage = other.m_aspectRatioPage;
    m_absoluteAreaPage = other.m_absoluteAreaPage;

    return *this;
}

LayoutTree::~LayoutTree()
{
    delete m_root;
}

int LayoutTree::addImage(double aspectRatio, double relativeArea)
{
    int index = m_count;

    if (!m_root)
    {
        m_root = new LayoutNode(aspectRatio, relativeArea, index);
        m_count++;
        return index;
    }

    // Section 2.1
    LayoutNode *bestTree = 0;
    double highScore = 0;
    for (int i=0; i< m_count; i++)
    {
        for (int horizontal=0; horizontal<2; horizontal++)
        {
            // create temporary tree
            LayoutNode *candidateTree = new LayoutNode(*m_root);

            // select the subtree which will be replace by a new internal node
            LayoutNode *candidateSubtree = candidateTree->nodeForIndex(i);

            // find parent node
            LayoutNode *parentNode = candidateTree->parentOf(candidateSubtree);

            // create new terminal node
            LayoutNode *newTerminalNode = new LayoutNode(aspectRatio, relativeArea, index);

            // create new internal node
            LayoutNode *newInternalNode = new LayoutNode(candidateSubtree, newTerminalNode, horizontal, index+1);

            // replace in tree
            if (parentNode)
                parentNode->takeAndSetChild(candidateSubtree, newInternalNode);
            else // candidateTree is candidateSubtree is root
                candidateTree = newInternalNode;

            // recompute sizes
            candidateTree->computeRelativeSizes();

            double candidateScore = score(candidateTree, m_count+2);
            if (candidateScore > highScore)
            {
                highScore = candidateScore;
                delete bestTree;
                bestTree = candidateTree;
            }
            else
            {
                delete candidateTree;
            }
        }
    }

    delete m_root;
    m_root = bestTree;
    m_root->computeDivisions();

    m_count += 2;
    return index;
}

// Section 2.2.1
double LayoutTree::score(LayoutNode *root, int nodeCount)
{
    if (!root)
        return 0;

    double areaSum = 0;
    for (int i = 0; i<nodeCount; i++)
    {
        LayoutNode *node = root->nodeForIndex(i);
        if (node->type() == LayoutNode::TerminalNode)
            areaSum += node->relativeArea();
    }

    double minRatioPage = root->aspectRatio() < m_aspectRatioPage ? root->aspectRatio() : m_aspectRatioPage;
    double maxRatioPage = root->aspectRatio() > m_aspectRatioPage ? root->aspectRatio() : m_aspectRatioPage;

    return G() * (areaSum / root->relativeArea()) * (minRatioPage / maxRatioPage);
}

// Section 2.2.2
double LayoutTree::G() const
{
    return 0.95 * 0.95;
}

// Section 2.2.2
double LayoutTree::absoluteArea(LayoutNode *node)
{
    // min(a_pbb, a_page), max(a_pbb, a_page)
    double minRatioPage = m_root->aspectRatio() < m_aspectRatioPage ? m_root->aspectRatio() : m_aspectRatioPage;
    double maxRatioPage = m_root->aspectRatio() > m_aspectRatioPage ? m_root->aspectRatio() : m_aspectRatioPage;

    // A_pbb
    double absoluteAreaRoot = m_absoluteAreaPage * minRatioPage / maxRatioPage;

    if (node == m_root)
        return absoluteAreaRoot;

    // A_i
    return G() * node->relativeArea() / m_root->relativeArea() * absoluteAreaRoot;
}

QRectF LayoutTree::drawingArea(int index, const QRectF &absoluteRectPage)
{
    LayoutNode *node = m_root->nodeForIndex(index);
    if (!node)
        return QRectF();

    // find out the "line of ancestry" of the node
    QList<LayoutNode*> treePath;
    LayoutNode *parent = node;
    while (parent)
    {
        treePath.prepend(parent);
        parent = m_root->parentOf(parent);
    }

    // find out the rect of the page bounding box (the rect of the root node in the page rect)
    QRectF absoluteRect = rectInRect(absoluteRectPage, m_root->aspectRatio(), absoluteArea(m_root));

    // go along the line of ancestry and narrow down the bounding rectangle,
    // as described in section 2.2.2
    for (int i=0; i<treePath.count() - 1; i++)
    {
        LayoutNode *parent = treePath[i];
        LayoutNode *child  = treePath[i+1]; // only iterating to count-1

        if (parent->type() == LayoutNode::VerticalDivision) // side by side
        {
            double leftWidth = absoluteRect.width() * parent->division();
            if (child == parent->leftChild())
            {
                absoluteRect.setWidth(leftWidth);
            }
            else // right child
            {
                double rightWidth = absoluteRect.width() - leftWidth;
                absoluteRect.setWidth(rightWidth);
                absoluteRect.translate(leftWidth, 0);
            }
        }
        else // horizontal division: one on top of the other
        {
            // left child is topmost
            double upperHeight = absoluteRect.height() * parent->division();
            if (child == parent->leftChild())
            {
                absoluteRect.setHeight(upperHeight);
            }
            else // right child
            {
                double lowerHeight = absoluteRect.height() - upperHeight;
                absoluteRect.setHeight(lowerHeight);
                absoluteRect.translate(0, upperHeight);
            }
        }
    }

    return rectInRect(absoluteRect, node->aspectRatio(), absoluteArea(node));
}

// lays out a rectangle with given aspect ratio and absolute area inside the given larger rectangle
// (not in the paper)
QRectF LayoutTree::rectInRect(const QRectF &rect, double aspectRatio, double absoluteArea)
{
    double width  = sqrt(absoluteArea / aspectRatio);
    double height = sqrt(absoluteArea * aspectRatio);
    double x      = rect.x() + (rect.width() - width) / 2;
    double y      = rect.y() + (rect.height() - height) / 2;
    return QRectF(x,y,width, height);
}

}  // NameSpace KIPIPrintImagesPlugin
