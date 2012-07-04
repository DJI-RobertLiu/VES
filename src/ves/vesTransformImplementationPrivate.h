#ifndef VESTRANSFORMPRIVATE_H
#define VESTRANSFORMPRIVATE_H

class vesTransformPrivate
{
private:
  vesMatrix4x4f T,C,R,SR,S,SRinv,Cinv;

public:
  vesMatrix4x4f Eval()
  {
    return T * C * R * SR * S * SRinv * Cinv;
  }

  void SetTranslation(vesVector3f trans)
  {
    T = makeTranslationMatrix4x4(trans);
  }

  void SetCenter(vesVector3f center)
  {
    C = makeTranslationMatrix4x4(center);
    Cinv = makeInverseMatrix4x4(C);
  }

  void SetRotation(vesVector4f rot)
  {
    R = makeRotationMatrix4x4(rot[3], rot[0], rot[1], rot[2]);
  }

  void SetScaleOrientation(vesVector4f sr)
  {
    SR = makeRotationMatrix4x4(sr[3], sr[0], sr[1], sr[2]);
    SRinv = makeInverseMatrix4x4(SR);
  }

  void SetScale(vesVector3f scale)
  {
    S = makeScaleMatrix4x4(scale[0],scale[1],scale[2]);
  }
};

#endif // VESTRANSFORMPRIVATE_H
