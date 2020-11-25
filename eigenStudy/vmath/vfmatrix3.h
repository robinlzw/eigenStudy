#ifndef __SCCMN_Matrix3_H__
#define __SCCMN_Matrix3_H__

//#include "vfvector3.h"
#include "vmathpredef.h"
#include <memory>



// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.geometrictools.com/

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.
 

/** A 3x3 matrix which can represent rotations around axes.
@note
<b>All the code is adapted from the Wild Magic 0.2 Matrix
library (http://www.geometrictools.com/).</b>
@par
The coordinate system is assumed to be <b>right-handed</b>.
*/





struct VFMatrix3
{
public :
    static const unsigned int ms_iSvdMaxIterations = 32 ;

public:
    /** Default constructor.
    @note
    It does <b>NOT</b> initialize the matrix for efficiency.
    */
    inline VFMatrix3 () {}
    inline explicit VFMatrix3 (const float arr[3][3])
    {
        memcpy(m,arr,9*sizeof(float));
    }
    inline VFMatrix3 (const VFMatrix3& rkMatrix)
    {
        memcpy(m,rkMatrix.m,9*sizeof(float));
    }
    VFMatrix3 (float fEntry00, float fEntry01, float fEntry02,
                float fEntry10, float fEntry11, float fEntry12,
                float fEntry20, float fEntry21, float fEntry22)
    {
        m[0][0] = fEntry00;
        m[0][1] = fEntry01;
        m[0][2] = fEntry02;
        m[1][0] = fEntry10;
        m[1][1] = fEntry11;
        m[1][2] = fEntry12;
        m[2][0] = fEntry20;
        m[2][1] = fEntry21;
        m[2][2] = fEntry22;
    }

    /** Exchange the contents of this matrix with another. 
    */
    inline void swap(VFMatrix3& other)
    {
        std::swap(m[0][0], other.m[0][0]);
        std::swap(m[0][1], other.m[0][1]);
        std::swap(m[0][2], other.m[0][2]);
        std::swap(m[1][0], other.m[1][0]);
        std::swap(m[1][1], other.m[1][1]);
        std::swap(m[1][2], other.m[1][2]);
        std::swap(m[2][0], other.m[2][0]);
        std::swap(m[2][1], other.m[2][1]);
        std::swap(m[2][2], other.m[2][2]);
    }

    // member access, allows use of construct mat[r][c]
    inline float* operator[] (size_t iRow) const
    {
        return (float*)m[iRow];
    }
    /*inline operator float* ()
    {
    return (float*)m[0];
    }*/
    VFVector3 GetColumn (size_t iCol) const;
    void SetColumn(size_t iCol, const VFVector3& vec);
    void FromAxes(const VFVector3& xAxis, const VFVector3& yAxis, const VFVector3& zAxis);

    // assignment and comparison
    inline VFMatrix3& operator= (const VFMatrix3& rkMatrix)
    {
        memcpy(m,rkMatrix.m,9*sizeof(float));
        return *this;
    }
    bool operator== (const VFMatrix3& rkMatrix) const;
    inline bool operator!= (const VFMatrix3& rkMatrix) const
    {
        return !operator==(rkMatrix);
    }

    // arithmetic operations
    VFMatrix3 operator+ (const VFMatrix3& rkMatrix) const;
    VFMatrix3 operator- (const VFMatrix3& rkMatrix) const;
    VFMatrix3 operator* (const VFMatrix3& rkMatrix) const;
    VFMatrix3 operator- () const;

    // matrix * vector [3x3 * 3x1 = 3x1]
    VFVector3 operator* (const VFVector3& rkVector) const;

    // vector * matrix [1x3 * 3x3 = 1x3]
     friend VFVector3 operator* ( const VFVector3& rkVector, const VFMatrix3& rkMatrix ) ;

    // matrix * scalar
    VFMatrix3 operator* (float fScalar) const;

    // scalar * matrix
     friend VFMatrix3 operator* (float fScalar, const VFMatrix3& rkMatrix);

    // utilities
    VFMatrix3 Transpose () const;
    bool Inverse (VFMatrix3& rkInverse, float fTolerance = 1e-06) const;
    VFMatrix3 Inverse (float fTolerance = 1e-06) const;
    float Determinant () const;

    // singular value decomposition
    void SingularValueDecomposition (VFMatrix3& rkL, VFVector3& rkS,
        VFMatrix3& rkR) const;
    void SingularValueComposition (const VFMatrix3& rkL,
        const VFVector3& rkS, const VFMatrix3& rkR);

    // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
    void Orthonormalize ();

    // orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
    void QDUDecomposition (VFMatrix3& rkQ, VFVector3& rkD,
        VFVector3& rkU) const;

    float SpectralNorm () const;

    // matrix must be orthonormal
    void ToAxisAngle (VFVector3& rkAxis, VFRadian & rfAngle) const;
    inline void ToAxisAngle (VFVector3& rkAxis, VFDegree& rfAngle) const {
        VFRadian r;
        ToAxisAngle ( rkAxis, r );
        rfAngle = r;
    }
    void FromAxisAngle (const VFVector3& rkAxis, const VFRadian& fRadians);

    // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
    // where yaw is rotation about the Up vector, pitch is rotation about the
    // Right axis, and roll is rotation about the Direction axis.
    bool ToEulerAnglesXYZ (VFRadian& rfYAngle, VFRadian& rfPAngle,
        VFRadian& rfRAngle) const;
    bool ToEulerAnglesXZY (VFRadian& rfYAngle, VFRadian& rfPAngle,
        VFRadian& rfRAngle) const;
    bool ToEulerAnglesYXZ (VFRadian& rfYAngle, VFRadian& rfPAngle,
        VFRadian& rfRAngle) const;
    bool ToEulerAnglesYZX (VFRadian& rfYAngle, VFRadian& rfPAngle,
        VFRadian& rfRAngle) const;
    bool ToEulerAnglesZXY (VFRadian& rfYAngle, VFRadian& rfPAngle,
        VFRadian& rfRAngle) const;
    bool ToEulerAnglesZYX (VFRadian& rfYAngle, VFRadian& rfPAngle,
        VFRadian& rfRAngle) const;
    void FromEulerAnglesXYZ (const VFRadian& fYAngle, const VFRadian& fPAngle, const VFRadian& fRAngle);
    void FromEulerAnglesXZY (const VFRadian& fYAngle, const VFRadian& fPAngle, const VFRadian& fRAngle);
    void FromEulerAnglesYXZ (const VFRadian& fYAngle, const VFRadian& fPAngle, const VFRadian& fRAngle);
    void FromEulerAnglesYZX (const VFRadian& fYAngle, const VFRadian& fPAngle, const VFRadian& fRAngle);
    void FromEulerAnglesZXY (const VFRadian& fYAngle, const VFRadian& fPAngle, const VFRadian& fRAngle);
    void FromEulerAnglesZYX (const VFRadian& fYAngle, const VFRadian& fPAngle, const VFRadian& fRAngle);
    // eigensolver, matrix must be symmetric
    void EigenSolveSymmetric (float afEigenvalue[3],
        VFVector3 akEigenvector[3]) const;

    static void TensorProduct (const VFVector3& rkU, const VFVector3& rkV,
        VFMatrix3& rkProduct);

    /** Determines if this matrix involves a scaling. */
    inline bool hasScale() const
    {
        // check magnitude of column vectors (==local axes)
        float t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
        if (!vf_equal_real(t, 1.0, (float)1e-04))
            return true;
        t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
        if (!vf_equal_real(t, 1.0, (float)1e-04))
            return true;
        t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
        if (!vf_equal_real(t, 1.0, (float)1e-04))
            return true;

        return false;
    } 

protected:
    // support for eigensolver
    void Tridiagonal (float afDiag[3], float afSubDiag[3]);
    bool QLAlgorithm (float afDiag[3], float afSubDiag[3]);

    // support for singular value decomposition 
    static void Bidiagonalize (VFMatrix3& kA, VFMatrix3& kL,
        VFMatrix3& kR);
    static void GolubKahanStep (VFMatrix3& kA, VFMatrix3& kL,
        VFMatrix3& kR);

    // support for spectral norm
    static float MaxCubicRoot (float afCoeff[3]);

    float m[3][3]; 

    // for faster access
    friend class VFMatrix4 ;
};
 
typedef VFMatrix3 VFMATRIX3 , * LPVFMATRIX3 ;

const VFMatrix3 MAT3_ZERO(0,0,0,0,0,0,0,0,0);
const VFMatrix3 MAT3_IDENTITY(1,0,0,0,1,0,0,0,1);
 
#endif
