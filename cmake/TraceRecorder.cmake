include(FetchContent)

FetchContent_Declare(
  TraceRecorder
  GIT_REPOSITORY https://eng-git.canterbury.ac.nz/nro55/tiva-libraries
  GIT_TAG        TraceRecorder
)

FetchContent_MakeAvailable(TraceRecorder)