"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.main = main;
async function main(prisma) {
    await prisma.user.deleteMany();
    await prisma.user.create({ data: { email: 'test@email.com' } });
    const usersCount = await prisma.user.count();
    console.log({ users_count: usersCount });
}
