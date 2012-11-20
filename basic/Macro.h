#ifndef DOF_MACRO_H_
#define DOF_MACRO_H_

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

#define STATIC_MEMBER_STRING(memberName, memberValue)\
inline static string memberName() {\
    static string __value(memberValue);\
    return __value;\
}

#define COUNT_TIME_START(id) \
    ptime start##id = second_clock::local_time();

#define COUNT_TIME_END(id) \
    ptime end##id = second_clock::local_time();\
    time_duration td##id = end##id - start##id;\
    cout << #id << " time use:" << to_simple_string(td##id) << endl;

#endif // DOF_MACRO_H_
