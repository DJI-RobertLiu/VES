
#include "vesMaterial.h"

// C++ includes
#include <map>


class vesMaterial::vesInternal
{
public:
  typedef std::map<vesMaterialAttribute::AttributeType,
    vesMaterialAttribute*> Attributes;

  Attributes m_attributes;
};


vesMaterial::vesMaterial() : vsgNode()
{
  this->m_internal = new vesInternal;
}


vesMaterial::~vesMaterial()
{
  delete this->m_internal; this->m_internal = 0x0;
}


bool vesMaterial::addAttribute(vesMaterialAttribute *attribute)
{
  if (!attribute) {
    return false;
  }

  if (attribute->type() != vesMaterialAttribute::Texture) {
    vesInternal::Attributes::iterator itr =
      this->m_internal->m_attributes.find( attribute->type() );

    if (itr == this->m_internal->m_attributes.end() || ( (itr->second) != attribute )) {
      this->m_internal->m_attributes[attribute->type()] = attribute;
    }
  }
}


vesMaterialAttribute* vesMaterial::attribute(vesMaterialAttribute::AttributeType type)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.find( type );

  if (itr != this->m_internal->m_attributes.end()) {
    return (itr->second);
  }

  return 0x0;
}


void vesMaterial::render(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->bind(renderState);
  }
}


void vesMaterial::remove(const vesRenderState &renderState)
{
  // \todo: Implement this.
}


void vesMaterial::setup(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {

    itr->second->setup(renderState);
  }
}


void vesMaterial::bind(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->bind(renderState);
  }
}


void vesMaterial::unbind(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->unbind(renderState);
  }
}


void vesMaterial::setupVertexData(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->setupVertexData(renderState);
  }
}


void vesMaterial::bindVertexData(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->bindVertexData(renderState);
  }
}


void vesMaterial::unbindVertexData(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->unbindVertexData(renderState);
  }
}
