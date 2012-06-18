#ifndef GETSET_H_
#define GETSET_H_

#define SETTER_GETTER_RETURN_VALUE(funcNameSuffix, typeName, memberName)\
inline typeName Get##funcNameSuffix() {\
    return (memberName); \
} \
inline void Set##funcNameSuffix(const typeName& (value)) {\
    (memberName) = (value); \
} 

#define SETTER_GETTER_RETURN_REF(funcNameSuffix, typeName, memberName)\
inline typeName& Get##funcNameSuffix() {\
    return (memberName); \
} \
inline void Set##funcNameSuffix(const typeName& (value)) {\
    (memberName) = (value); \
} 

#define SETTER_GETTER_RETURN_CONST_REF(funcNameSuffix, typeName, memberName)\
inline const typeName& Get##funcNameSuffix() const {\
    return (memberName); \
} \
inline void Set##funcNameSuffix(const typeName& (value)) {\
    (memberName) = (value); \
} 

#define GETTER_RETURN_VALUE(funcNameSuffix, typeName, memberName)\
inline typeName Get##funcNameSuffix() {\
    return (memberName); \
}

#define GETTER_RETURN_REF(funcNameSuffix, typeName, memberName)\
inline typeName& Get##funcNameSuffix() {\
    return (memberName); \
}

#define GETTER_RETURN_CONST_REF(funcNameSuffix, typeName, memberName)\
inline const typeName& Get##funcNameSuffix() const {\
    return (memberName); \
}

#endif // GETSET_H_
