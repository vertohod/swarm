#include "service_messages.h"
#include "util.h"

std::unordered_map<int, std::string> service_messages = {
    {SMESS000_OK, "Ok"},
    {SMESS001_ERROR, "Error"},
    {SMESS100_WELCOME, "Welcome to anonymous chat"},
    {SMESS101_USER_CAME_IN, "Let's welcome %s to the channel."},
    {SMESS102_USER_GOT_OUT, "The user %s has left the channel."},
    {SMESS103_LEFT_WITHOUT_GB, "The user %s has left without saying goodbye."},
    {SMESS104_NICKNAME_WRONG, "A nickname can't contain less than %s or more %s characters."},
    {SMESS105_NICKNAME_EXIST, "This nickname is taken already. Choose another one please."},
    {SMESS106_ROOMNAME_WRONG, "A channel's name can't contain less than %s or more %s characters."},
    {SMESS107_MESSAGE_WRONG, "A message can't be empty or exceed %s characters."},
    {SMESS108_CHANGE_NICKNAME, "The user %s has changed his nickname to %s."},
    {SMESS109_MESSAGE_FLOOD, "Don't send the same messages please."},
    {SMESS110_CHANGED_ROOM, "The user %s has joined the channel %s."},
    {SMESS111_INVALID_CHARACTERS, "You have used invalid characters. The name can contain letters, numbers, dashes and underscores."},
    {SMESS112_ACCESS_ADDED, "The user %s has granted you the permission to access the private channel %s."},
    {SMESS113_ACCESS_REMOVED, "Access to the channel %s was closed."},
    {SMESS114_USER_WAS_DRIVEN, "The user %s was driven from channel."},
    {SMESS115_NICKNAME_IS_FREE, "An alias for %s can’t be created as it doesn’t exist."},
    {SMESS116_PASSWORD_WRONG, "The password is incorrect."},
    {SMESS117_PASSWORD_ADDED, "A password on your session is installed. Now you can run it on another device."},
    {SMESS118_THANKS_FOR_MESSAGE, "Thank you for your message. We will read it."},
    {SMESS119_NOT_SUPPORTED, "Uploaded file is not supported."},
    {SMESS120_IMAGE_UPLOADED, "The image was uploaded successfully. Please wait while it's being processed."},
    {SMESS121_AUDIO_UPLOADED, "The audio file was uploaded successfully."},
    {SMESS122_UNKNOWN_EXTENSION, "Unknown file's extension."},
    {SMESS123_UPLOAD_NOT_SUCCESS, "Upload complete isn't successfully."},
    {SMESS124_ROOM_ADDED, "The channel was added successfully."},
    {SMESS125_IMAGE_REMOVED, "The images were removed successfully."},
    {SMESS126_AUDIO_REMOVED, "The audio files were removed successfully."},
    {SMESS127_ALLOW_REMOVED, "The permission for channel was removed successfully."},
    {SMESS128_MESSAGE_REMOVED, "The messages were removed successfully."},
    {SMESS129_USER_BLOCKED, "The user %s was blocked."},
    {SMESS130_ROOMNAME_EXIST, "This channel's name is taken already. Choose another one please."},
    {SMESS131_AVATAR_INSTALLED, "An avatar installed successfully"},
    {SMESS132_INVITATION_ADDED, "Invitation for the channel %s was added"},
    {SMESS133_USED_INVITATION, "You have used invatation for coming in the channel %s"},
    {SMESS134_FAILUSE_INVITATION, "You can't use this invitation."},
    {SMESS135_VIDEO_UPLOADED, "The video file was uploaded successfully."},
    {SMESS136_VIDEO_REMOVED, "The video files were removed successfully."},
    {SMESS137_ARCHIVE_UPLOADED, "The file was uploaded successfully."},
    {SMESS138_ARCHIVE_REMOVED, "The archive files were removed successfully."},
    {SMESS201_UNAUTHORIZED, "An user is not logged."},
    {SMESS202_USER_NOT_INSIDE, "An user haven't joined a channel, and can't sends messages here."},
    {SMESS203_IMAGE_DN_BELONG, "The picture with the specified name does not belong to user."},
    {SMESS204_ROOM_DN_BELONG, "The channel with the specified name does not belong to user."},
    {SMESS205_IMAGE_NOT_FOUND, "The picture with the specified name is not found."},
    {SMESS206_USER_NOT_FOUND, "An user with the ID is not found."},
    {SMESS207_ROOM_ACCESS_DENIED, "The channel %s is private you don't have permission to access it."},
    {SMESS208_IMAGE_ACCESS_DENIED, "Access denied to image %s"},
    {SMESS209_AUDIO_ACCESS_DENIED, "Access denied to audio file %s"},
    {SMESS210_VIDEO_ACCESS_DENIED, "Access denied to video file %s"},
    {SMESS211_ARCHIVE_ACCESS_DENIED, "Access denied to file %s"},
    {SMESS212_SESSION_NOT_FOUND, "The session was not found 8-/"},
    {SMESS213_USERID_NOT_FOUND, "System user's id was not found for %s"},
    {SMESS214_GROUPID_NOT_FOUND, "System group's id was not found for %s"},
    {SMESS215_SET_UNABLE, "Failed to establish rights for %s"},
    {SMESS216_CHANGE_UNABLE, "Failed to change mode for %s"},
    {SMESS217_DELETE_UNABLE, "Failed to delete file %s"},
    {SMESS218_AUDIO_DN_BELONG, "The audio file with the specified name does not belong to user"},
    {SMESS219_ADD_USER_UNABLE, "Failed to add new session"},
    {SMESS220_OBJECT_NOT_FOUND, "The object %s was not found"},
    {SMESS221_VIDEO_DN_BELONG, "The video file with the specified name does not belong to user"},
    {SMESS222_ARCHIVE_DN_BELONG, "The archive with the specified name does not belong to user"}
};

sptr_cstr smessage(int code, const std::string& str1, const std::string& str2, const std::string& str3)
{
    sptr_str res;

    auto it = service_messages.find(code);
    if (it != service_messages.end()) {
        *res = util::ins(it->second, str1);
        *res = util::ins(*res, str2);
        *res = util::ins(*res, str3);

//        *res = format(code) + " " + *res + ";" + str1 + ";" + str2 + ";" + str3;
        *res = *res + ";" + str1 + ";" + str2 + ";" + str3;
    }

    return res;
}

sptr_cstr smessage(int code, const std::string& str1, const std::string& str2)
{
    sptr_str res;

    auto it = service_messages.find(code);
    if (it != service_messages.end()) {
        *res = util::ins(it->second, str1);
        *res = util::ins(*res, str2);

//        *res = format(code) + " " + *res + ";" + str1 + ";" + str2;
        *res = *res + ";" + str1 + ";" + str2;
    }

    return res;
}

sptr_cstr smessage(int code, const std::string& str)
{
    sptr_str res;

    auto it = service_messages.find(code);
    if (it != service_messages.end()) {
//        *res = format(code) + " " + util::ins(it->second, str) + ";" + str;
        *res = util::ins(it->second, str) + ";" + str;
    }

    return res;
}

sptr_cstr smessage(int code)
{
    sptr_str res;

    auto it = service_messages.find(code);
    if (it != service_messages.end()) 
//        *res = format(code) + " " + it->second;
        *res = it->second;

    return res;
}
