# qgit - A simplified git like version control system
# Copyright (C) 2025 - 2026 Qiu Yixiang
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# configure for debug mode
DEBUG := 1

# output executable and library name
BIN_NAME := qgit
LIB_NAME := qgit

CC := gcc
CXX := g++
AR := ar
LD := gcc

LIB_BUILD ?= static