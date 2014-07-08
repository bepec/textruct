#!/bin/bash
cpplint.py --root=include --filter=-runtime/references,-build/include include/* unittest/*
