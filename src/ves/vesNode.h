/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#ifndef __VESNODE_H
#define __VESNODE_H

#include "vesBoundingObject.h"
#include "vesObject.h"

// VES includes
#include "vesSetGet.h"

// Forward declarations.
class vesActor;
class vesCamera;
class vesGroupNode;
class vesMaterial;
class vesTransformNode;
class vesVisitor;

class vesNode : public vesObject, public vesBoundingObject
{
public:
  vesTypeMacro(vesNode);

  vesNode();
  virtual ~vesNode();

  /// \brief Accept visitor for scene traversal
  virtual void accept(vesVisitor &visitor);

  /// \brief Set material to be used for the node
  void setMaterial(vesSharedPtr<vesMaterial> material);
  vesSharedPtr<vesMaterial> material() { return this->m_material; }
  const vesSharedPtr<vesMaterial> material() const { return this->m_material; }

  /// \brief Set parent of this node
  bool setParent(vesGroupNode *parent);
  vesGroupNode* parent(){ return this->m_parent; }

  /// \brief Set if this node is a overlay node. Overlay nodes are drawn
  /// on top of scene nodes.
  inline void setIsOverlayNode(bool value) { this->m_isOverlayNode = value; }
  inline bool isOverlayNode() const { return this->m_isOverlayNode; }

  /// \brief Set if this node should be visible
  void setVisible(bool value);
  bool isVisible() const { return this->m_visible; }

  virtual vesGroupNode* asGroupNode() { return 0x0; }
  virtual const vesGroupNode* asGroupNode() const { return 0x0; }
  virtual vesTransformNode* asTransformNode() { return 0x0; }
  virtual const vesTransformNode* asTransformNode() const { return 0x0; }
  virtual vesCamera* asCamera() { return 0x0; }
  virtual const vesCamera* asCamera() const { return 0x0; }
  virtual vesActor* asActor() { return 0x0; }
  virtual const vesActor* asActor() const { return 0x0; }

  /// \brief Traverse parent
  virtual void ascend(vesVisitor &visitor)
    { vesNotUsed(visitor); }

  /// \brief Traverse children
  virtual void traverse(vesVisitor &visitor)
    { vesNotUsed(visitor); }

protected:
  virtual void computeBounds();
  virtual void updateBounds(vesNode &child)
    { vesNotUsed(child); }

  bool m_visible;
  bool m_isOverlayNode;

  vesGroupNode* m_parent;

  vesSharedPtr<vesMaterial> m_material;
};

#endif // __VESNODE_H
