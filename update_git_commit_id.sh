#!/bin/sh

echo "#define GIT_COMMIT_ID \"`git rev-parse HEAD`\"" > git_commit_id.h
