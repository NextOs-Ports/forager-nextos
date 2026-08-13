#include <stdio.h>
#include <vector>
#include <set>
#include <string>

#include "so_util.h"
#include "jni.h"
#include "jni_internals.h"
#include "RunnerJNILib.h"
#include "classes/lang_Double.h"
#include "libyoyo.h"
#include "jni/classes/bytebuffer.h"
#include "video.h"

#define MANGLED_CLASSPATH "Java_com_yoyogames_runner_RunnerJNILib_"
#define CLASS RunnerJNILib

/*
    Native Function Pointers
*/

decltype(RunnerJNILib::Startup)                  RunnerJNILib::Startup = NULL;
decltype(RunnerJNILib::CreateVersionDSMap)       RunnerJNILib::CreateVersionDSMap = NULL;
decltype(RunnerJNILib::Process)                  RunnerJNILib::Process = NULL;
decltype(RunnerJNILib::TouchEvent)               RunnerJNILib::TouchEvent = NULL;
decltype(RunnerJNILib::RenderSplash)             RunnerJNILib::RenderSplash = NULL;
decltype(RunnerJNILib::Resume)                   RunnerJNILib::Resume = NULL;
decltype(RunnerJNILib::Pause)                    RunnerJNILib::Pause = NULL;
decltype(RunnerJNILib::KeyEvent)                 RunnerJNILib::KeyEvent = NULL;
decltype(RunnerJNILib::SetKeyValue)              RunnerJNILib::SetKeyValue = NULL;
decltype(RunnerJNILib::GetAppID)                 RunnerJNILib::GetAppID = NULL;
decltype(RunnerJNILib::GetSaveFileName)          RunnerJNILib::GetSaveFileName = NULL;
decltype(RunnerJNILib::ExpandCompressedFile)     RunnerJNILib::ExpandCompressedFile = NULL;
decltype(RunnerJNILib::iCadeEventDispatch)       RunnerJNILib::iCadeEventDispatch = NULL;
decltype(RunnerJNILib::registerGamepadConnected) RunnerJNILib::registerGamepadConnected = NULL;
decltype(RunnerJNILib::initGLFuncs)              RunnerJNILib::initGLFuncs = NULL;
decltype(RunnerJNILib::canFlip)                  RunnerJNILib::canFlip = NULL;
decltype(RunnerJNILib::GCMPushResult)            RunnerJNILib::GCMPushResult = NULL;

#define NATIVE_METHOD(so, sym) {&CLASS::clazz, MANGLED_CLASSPATH #sym, so, (void**)&CLASS::sym}
const NativeMethod RunnerJNILibNativeMethods[] = {
    NATIVE_METHOD("libyoyo.so", Startup),
    NATIVE_METHOD("libyoyo.so", CreateVersionDSMap),
    NATIVE_METHOD("libyoyo.so", Process),
    NATIVE_METHOD("libyoyo.so", TouchEvent),
    NATIVE_METHOD("libyoyo.so", RenderSplash),
    NATIVE_METHOD("libyoyo.so", Resume),
    NATIVE_METHOD("libyoyo.so", Pause),
    NATIVE_METHOD("libyoyo.so", KeyEvent),
    NATIVE_METHOD("libyoyo.so", SetKeyValue),
    NATIVE_METHOD("libyoyo.so", GetAppID),
    NATIVE_METHOD("libyoyo.so", GetSaveFileName),
    NATIVE_METHOD("libyoyo.so", ExpandCompressedFile),
    NATIVE_METHOD("libyoyo.so", iCadeEventDispatch),
    NATIVE_METHOD("libyoyo.so", registerGamepadConnected),
    NATIVE_METHOD("libyoyo.so", initGLFuncs),
    NATIVE_METHOD("libyoyo.so", canFlip),
    NATIVE_METHOD("libyoyo.so", GCMPushResult),
    {NULL}
};

/*
    Static Class Members
*/

double RunnerJNILib::mGameSpeedControl = 60.0;

const FieldId RunnerJNILibFields[] = {
    REGISTER_STATIC_FIELD(RunnerJNILib, mGameSpeedControl),
    {NULL},
};

/*
    Managed Class Methods
*/

jfloatArray RunnerJNILib::GamepadAxesValues(JNIEnv *env, jclass clz, void *ins, jint deviceIndex, jstring test)
{
    return 0;
}

/* [NextOS/forager] O runner pede pro lado Java limpar/enumerar os pads; aqui os pads
 * chegam pelo SDL_GameController + eventos assincronos do input.cpp — nada a fazer.
 * Stubs no-op tiram os "does not have static method" do log. */
void RunnerJNILib::ClearGamepads(JNIEnv *env, jclass clz)
{
}

void RunnerJNILib::EnumerateGamepadDevices(JNIEnv *env, jclass clz)
{
}

extern int RunnerJNILib_MoveTaskToBackCalled;
void RunnerJNILib::MoveTaskToBack(JNIEnv *env, jclass clz)
{
    RunnerJNILib_MoveTaskToBackCalled = 1;
    warning("MoveTaskToBack called.\n");
}

int RunnerJNILib::OsGetInfo(JNIEnv *env, jclass clz)
{
    static String osinfo_arr[] = {
        /* "RELEASE", */       String("v1.0"),
        /* "MODEL", */         String("Homebrew"),
        /* "DEVICE", */        String("Homebrew"),
        /* "MANUFACTURER", */  String("JohnnyonFlame"),
        /* "CPU_ABI", */       String("armeabi-v7a"),
        /* "CPU_ABI2", */      String("arm64-v8a"),
        /* "BOOTLOADER", */    String("U-Boot"),
        /* "BOARD", */         String("You tell me"),
        /* "VERSION", */       String("v1.0"),
        /* "REGION", */        String("Global"),
        /* "VERSION_NAME", */  String("v1.0"),
    };

    int osinfo = RunnerJNILib::CreateVersionDSMap(env, NULL, 0x13,
        &osinfo_arr[0], &osinfo_arr[1], &osinfo_arr[2], &osinfo_arr[3], &osinfo_arr[4], &osinfo_arr[5],
        &osinfo_arr[6], &osinfo_arr[7], &osinfo_arr[8], &osinfo_arr[9], &osinfo_arr[10], (jboolean)1);
    warning(" -- Retuning OsInfo %d. --\n", osinfo);
    return osinfo;
}

/* [NextOS/forager] CallExtensionFunction — a ponte pela qual o GameMaker chama TODA
 * funcao de extensao Java. O Forager declara ForagerLicensing / GooglePlayLicensing /
 * GooglePlayBilling / GooglePlayAds / GooglePlayServices / BlitworksCloudSave /
 * PlayAPKExpansion / Notch. Sem este metodo o runner devolvia `undefined`, e o jogo
 * estourava em "unable to add undefined to a string" no Draw do WorldControl.
 *
 * 🚨 REGRA DA CASA: nenhum shim pode devolver FALHA. Devolvemos uma String VAZIA —
 * concatenar "" numa string sempre funciona, e o jogo segue offline em vez de morrer.
 * (Devolver NULL traria o `undefined` de volta; devolver um Real quebraria o concat.)
 *
 * O warning lista exatamente qual classe/metodo o jogo pediu: e a lista do que ainda
 * pode precisar de resposta ESPECIFICA (ex.: licenca = LICENSED).
 */
jobject RunnerJNILib::CallExtensionFunction(JNIEnv *env, jclass clz, jstring className,
                                            jstring methodName, jint argCount,
                                            jdoubleArray dArgs, jobjectArray oArgs)
{
    const char *cls = className  ? ((String *)className)->str  : "(null)";
    const char *mth = methodName ? ((String *)methodName)->str : "(null)";

    /* Ext_Call desembrulha o retorno via FindClass("java/lang/Double") + doubleValue().
     * Entao a resposta inerte tem que ser um Double, nao uma String: devolver String
     * (ou NULL) faz o valor virar `undefined` no GML. Zero e o neutro certo aqui —
     * NOTCH_getSafeInset* = sem entalhe, GPBilling_Init = sem compra, etc. */
    static LangDouble zero(0.0);

    /* Logar so a PRIMEIRA vez de cada classe.metodo: NOTCH_getSafeInset* e chamado
     * todo frame e enchia o log com milhares de linhas iguais, escondendo o resto. */
    static std::set<std::string> ja_logado;
    std::string chave = std::string(cls) + "." + mth;
    if (ja_logado.insert(chave).second)
        warning("CallExtensionFunction: %s.%s (argc=%d) -> 0 (inerte, offline)\n",
                cls, mth, (int)argCount);

    return (jobject)&zero;
}

#ifdef VIDEO_SUPPORT
void RunnerJNILib::VideoOpen(JNIEnv *env, jclass clz, jstring path)
{
    video_open_internal(env->GetStringUTFChars(path,NULL));
}

void RunnerJNILib::VideoClose(JNIEnv *env, jclass clz)
{
    video_close_internal();
}

jboolean RunnerJNILib::VideoDraw(JNIEnv *env, jclass clz, jobject bytebuffer)
{
    ByteBuffer* buf=(ByteBuffer*)bytebuffer;
    return video_draw_internal(buf->address());
}

void RunnerJNILib::VideoSetVolume(JNIEnv *env, jclass clz, jdouble volume)
{
    return video_set_volume_internal(volume);
}

void RunnerJNILib::VideoSeekTo(JNIEnv *env, jclass clz, jdouble time)
{
    return video_seek_to_internal(time);
}

void RunnerJNILib::VideoEnableLoop(JNIEnv *env, jclass clz, jdouble loop)
{
    return video_enable_loop_internal(loop);
}

void RunnerJNILib::VideoPause(JNIEnv *env, jclass clz)
{
    return video_pause_internal();
}

void RunnerJNILib::VideoResume(JNIEnv *env, jclass clz)
{
    return video_resume_internal();
}

jdouble RunnerJNILib::VideoStatus(JNIEnv *env, jclass clz)
{
    return video_status_internal();
}

jdouble RunnerJNILib::VideoGetStatus(JNIEnv *env, jclass clz)
{
    return video_get_status_internal();
}

jdouble RunnerJNILib::VideoGetFormat(JNIEnv *env, jclass clz)
{
    return video_get_format_internal();
}

jdouble RunnerJNILib::VideoW(JNIEnv *env, jclass clz)
{
    return video_get_width_internal();
}

jdouble RunnerJNILib::VideoH(JNIEnv *env, jclass clz)
{
    return video_get_height_internal();
}

jdouble RunnerJNILib::VideoGetDuration(JNIEnv *env, jclass clz)
{
    return video_get_duration_internal();
}

jdouble RunnerJNILib::VideoGetPosition(JNIEnv *env, jclass clz)
{
    return video_get_position_internal();
}

jdouble RunnerJNILib::VideoGetVolume(JNIEnv *env, jclass clz)
{
    return video_get_volume_internal();
}

jdouble RunnerJNILib::VideoIsLooping(JNIEnv *env, jclass clz)
{
    return video_is_looping_internal();
}



#endif // VIDEO_SUPPORT

const ManagedMethod RunnerJNILibManagedMethods[] = {
    REGISTER_STATIC_METHOD(RunnerJNILib, OsGetInfo, "()I"),
    REGISTER_STATIC_METHOD(RunnerJNILib, CallExtensionFunction, "(Ljava/lang/String;Ljava/lang/String;I[D[Ljava/lang/Object;)Ljava/lang/Object;"),
    REGISTER_STATIC_METHOD(RunnerJNILib, GamepadAxesValues, "(I)[F"),
    REGISTER_STATIC_METHOD(RunnerJNILib, MoveTaskToBack, "()V"),
    REGISTER_STATIC_METHOD(RunnerJNILib, ClearGamepads, "()V"),
    REGISTER_STATIC_METHOD(RunnerJNILib, EnumerateGamepadDevices, "()V"),
    #ifdef VIDEO_SUPPORT
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoOpen, "(Ljava/lang/String;)V"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoClose, "()V"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoDraw, "(Ljava/nio/ByteBuffer;)Z"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoSetVolume, "(D)V"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoEnableLoop, "(D)V"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoSeekTo, "(D)V"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoPause, "()V"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoResume, "()V"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoStatus, "()D"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoGetFormat, "()D"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoW, "()D"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoH, "()D"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoGetDuration, "()D"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoGetPosition, "()D"), 
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoGetStatus, "()D"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoIsLooping, "()D"),
    REGISTER_STATIC_METHOD(RunnerJNILib, VideoGetVolume, "()D"),
    #endif // VIDEO_SUPPORT
    NULL
};

Class RunnerJNILib::clazz = {
    .classpath = "com/yoyogames/runner/RunnerJNILib",
    .classname = "RunnerJNILib",
    .managed_methods = RunnerJNILibManagedMethods,
    .native_methods = RunnerJNILibNativeMethods,
    .fields = RunnerJNILibFields,
    .instance_size = sizeof(String)
};

static const int registered = ClassRegistry::register_class(RunnerJNILib::clazz);